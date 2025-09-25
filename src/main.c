#define _POSIX_C_SOURCE 200809L
#include "../headers/config.h"
#include "../headers/ui.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    // Check for setup mode
    if (argc > 1 && strcmp(argv[1], "--setup") == 0) {
        setup_keyboard_interactive();
        return 0;
    }

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s [OPTIONS]\n", argv[0]);
        printf("  --setup    Configure keyboard device\n");
        printf("  --help     Show this help\n");
        return 0;
    }

    AppData app_data = {0};

    GtkApplication *app = gtk_application_new("com.example.accent-popup",
                                             G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), &app_data);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Cleanup
    if (app_data.timer_id > 0) {
        g_source_remove(app_data.timer_id);
    }
    if (app_data.dev) {
        libevdev_free(app_data.dev);
    }
    if (app_data.kbd_fd >= 0) {
        close(app_data.kbd_fd);
    }
    if (app_data.keyboard_device) {
        free(app_data.keyboard_device);
    }

    g_object_unref(app);
    return status;
}