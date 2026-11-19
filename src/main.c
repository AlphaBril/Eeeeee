#define _POSIX_C_SOURCE 200809L
#include "../headers/keyboard.h"
#include "../headers/ui.h"
#include "../headers/input.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--setup") == 0) {
        printf("--setup is no longer needed: eeeeee listens to all keyboards automatically.\n");
        return 0;
    }

    if (argc > 1 && strcmp(argv[1], "--version") == 0) {
        printf("eeeeee %s\n", EEEEEE_VERSION);
        return 0;
    }

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s [OPTIONS]\n", argv[0]);
        printf("  --version  Show version\n");
        printf("  --help     Show this help\n");
        return 0;
    }

    // Initialize virtual keyboard for typing accents
    if (setup_virtual_keyboard() < 0) {
        printf("Error: Could not setup virtual keyboard for typing accents.\n");
        printf("Make sure /dev/uinput is accessible to input group.\n");
        return 1;
    }

    AppData app_data = {0};

    GtkApplication *app = gtk_application_new("com.example.accent-popup",
                                             G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), &app_data);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Cleanup
    cleanup_virtual_keyboard();

    if (app_data.timer_id > 0) {
        g_source_remove(app_data.timer_id);
    }
    cleanup_keyboard_monitoring(&app_data);

    g_object_unref(app);
    return status;
}
