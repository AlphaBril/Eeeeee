#define _POSIX_C_SOURCE 200809L
#include "../headers/keyboard.h"
#include "../headers/events.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#define INPUT_DIR "/dev/input"

static bool is_keyboard(struct libevdev *dev) {
    if (!libevdev_has_event_type(dev, EV_KEY) ||
        !libevdev_has_event_code(dev, EV_KEY, KEY_E)) {
        return false;
    }

    const char *name = libevdev_get_name(dev);
    if (!name) return true;

    return !(strstr(name, "Mouse") || strstr(name, "System Control") ||
             strstr(name, "Consumer Control") || strstr(name, "Touchpad") ||
             strcmp(name, VIRTUAL_KEYBOARD_NAME) == 0);
}

static void free_keyboard_source(gpointer data) {
    KeyboardSource *kbd = (KeyboardSource *)data;

    if (kbd->watch_id > 0) {
        g_source_remove(kbd->watch_id);
    }
    libevdev_free(kbd->dev);
    close(kbd->fd);
    g_free(kbd->path);
    g_free(kbd);
}

// Returns 0 when the device is (already) watched, otherwise a positive errno
// or -1 when the device is not a keyboard.
static int add_keyboard_device(AppData *app, const char *path) {
    if (g_hash_table_contains(app->keyboards, path)) return 0;

    int fd = open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (fd < 0) return errno;

    struct libevdev *dev;
    if (libevdev_new_from_fd(fd, &dev) < 0) {
        close(fd);
        return -1;
    }

    if (!is_keyboard(dev)) {
        libevdev_free(dev);
        close(fd);
        return -1;
    }

    KeyboardSource *kbd = g_new0(KeyboardSource, 1);
    kbd->app = app;
    kbd->path = g_strdup(path);
    kbd->fd = fd;
    kbd->dev = dev;

    GIOChannel *channel = g_io_channel_unix_new(fd);
    kbd->watch_id = g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_ERR,
                                   keyboard_event_handler, kbd);
    g_io_channel_unref(channel);

    g_hash_table_insert(app->keyboards, kbd->path, kbd);
    printf("✓ Listening to %s (%s)\n", libevdev_get_name(dev), path);
    return 0;
}

void remove_keyboard_device(AppData *app, KeyboardSource *kbd) {
    printf("Keyboard removed: %s\n", kbd->path);
    app->key_pressed = 0;
    g_hash_table_remove(app->keyboards, kbd->path);
}

static void on_input_dir_changed(
    GFileMonitor *monitor __attribute__((unused)),
    GFile *file,
    GFile *other_file __attribute__((unused)),
    GFileMonitorEvent event_type,
    gpointer user_data
) {
    AppData *app = (AppData *)user_data;
    char *name = g_file_get_basename(file);

    if (g_str_has_prefix(name, "event")) {
        char *path = g_file_get_path(file);

        // Permissions are applied by udev after the node is created, so
        // retry on attribute changes too.
        if (event_type == G_FILE_MONITOR_EVENT_CREATED ||
            event_type == G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED) {
            add_keyboard_device(app, path);
        } else if (event_type == G_FILE_MONITOR_EVENT_DELETED) {
            KeyboardSource *kbd = g_hash_table_lookup(app->keyboards, path);
            if (kbd) remove_keyboard_device(app, kbd);
        }

        g_free(path);
    }

    g_free(name);
}

bool setup_keyboard_monitoring(AppData *app) {
    app->keyboards = g_hash_table_new_full(g_str_hash, g_str_equal,
                                           NULL, free_keyboard_source);

    DIR *dir = opendir(INPUT_DIR);
    if (!dir) {
        printf("Error: Could not open %s: %s\n", INPUT_DIR, strerror(errno));
        return false;
    }

    bool permission_denied = false;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "event", 5) != 0) continue;

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", INPUT_DIR, entry->d_name);
        if (add_keyboard_device(app, path) == EACCES) {
            permission_denied = true;
        }
    }
    closedir(dir);

    if (g_hash_table_size(app->keyboards) == 0) {
        printf("No keyboard devices found!\n");
        if (permission_denied) {
            printf("Make sure you're in the 'input' group: sudo usermod -a -G input $USER\n");
        }
        return false;
    }

    GFile *input_dir = g_file_new_for_path(INPUT_DIR);
    GError *error = NULL;
    app->input_monitor = g_file_monitor_directory(input_dir, G_FILE_MONITOR_NONE,
                                                  NULL, &error);
    g_object_unref(input_dir);

    if (app->input_monitor) {
        g_signal_connect(app->input_monitor, "changed",
                         G_CALLBACK(on_input_dir_changed), app);
    } else {
        printf("⚠ Keyboard hotplug disabled: %s\n", error->message);
        g_error_free(error);
    }

    printf("✓ Keyboard monitoring active, waiting for input\n");
    return true;
}

void cleanup_keyboard_monitoring(AppData *app) {
    if (app->input_monitor) {
        g_object_unref(app->input_monitor);
        app->input_monitor = NULL;
    }
    if (app->keyboards) {
        g_hash_table_destroy(app->keyboards);
        app->keyboards = NULL;
    }
}
