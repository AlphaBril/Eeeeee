#define _POSIX_C_SOURCE 200809L
#include "../headers/keyboard.h"
#include "../headers/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

void free_keyboard_devices(KeyboardDevice *devices, int count) {
    for (int i = 0; i < count; i++) {
        free(devices[i].device_path);
        free(devices[i].device_name);
    }
    free(devices);
}

KeyboardDevice* scan_keyboard_devices(int *count) {
    DIR *dir;
    struct dirent *entry;
    char path[256];
    KeyboardDevice *devices = malloc(32 * sizeof(KeyboardDevice));
    *count = 0;

    dir = opendir("/dev/input");
    if (!dir) {
        free(devices);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL && *count < 32) {
        if (strncmp(entry->d_name, "event", 5) != 0) {
            continue;
        }

        snprintf(path, sizeof(path), "/dev/input/%.240s", entry->d_name);

        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        struct libevdev *dev;
        int rc = libevdev_new_from_fd(fd, &dev);
        if (rc < 0) {
            close(fd);
            continue;
        }

        if (libevdev_has_event_type(dev, EV_KEY) &&
            libevdev_has_event_code(dev, EV_KEY, KEY_E)) {

            const char *name = libevdev_get_name(dev);
            bool has_full_keyboard = libevdev_has_event_code(dev, EV_KEY, KEY_A) &&
                                   libevdev_has_event_code(dev, EV_KEY, KEY_Z);

            if (strstr(name, "Mouse") || strstr(name, "System Control") ||
                strstr(name, "Consumer Control") || strstr(name, "Touchpad")) {
                libevdev_free(dev);
                close(fd);
                continue;
            }

            devices[*count].device_path = strdup(path);
            devices[*count].device_name = strdup(name);
            devices[*count].has_full_keyboard = has_full_keyboard;
            (*count)++;
        }

        libevdev_free(dev);
        close(fd);
    }

    closedir(dir);
    return devices;
}

int find_keyboard_device(AppData *app) {
	int configured_time = DEFAULT_HOLD_DURATION_MS;
	char *configured_device = load_keyboard_config(&configured_time);
    if (configured_device) {
        printf("Using configured keyboard device: %s with %ims of interval time\n", configured_device, configured_time);

        int fd = open(configured_device, O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            struct libevdev *dev;
            int rc = libevdev_new_from_fd(fd, &dev);
            if (rc >= 0) {
                if (libevdev_has_event_type(dev, EV_KEY) &&
                    libevdev_has_event_code(dev, EV_KEY, KEY_E)) {
                    printf("✓ Configured device is working: %s\n", libevdev_get_name(dev));
                    app->keyboard_device = configured_device;
					app->configured_time = configured_time;
                    libevdev_free(dev);
                    return fd;
                }
                libevdev_free(dev);
            }
            close(fd);
        }

        printf("⚠ Configured device is not available: %s\n", configured_device);
        free(configured_device);
    }

    printf("No valid configuration found.\n");
    printf("Run 'accent-popup --setup' to configure your keyboard.\n");
    return -1;
}