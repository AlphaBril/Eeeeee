#ifndef TYPES_H
#define TYPES_H

#include <gtk/gtk.h>
#include <libevdev/libevdev.h>
#include <sys/time.h>
#include <stdbool.h>

#define DEFAULT_HOLD_DURATION_MS 500
#define CONFIG_DIR ".config/eeeeee"
#define CONFIG_FILE "config"
#define VIRTUAL_KEYBOARD_NAME "Accent Popup Virtual Keyboard"

typedef struct {
    GtkWidget *window;
    GtkWidget *popup_window;
    GtkWidget *popup_button_box;

    GHashTable *keyboards;
    GFileMonitor *input_monitor;
    int key_pressed;
	bool shift_key_hold;
	const char **current_accents;
    struct timeval press_time;
    guint timer_id;
    bool popup_shown;

	int configured_time;
} AppData;

typedef struct {
    AppData *app;
    char *path;
    int fd;
    struct libevdev *dev;
    guint watch_id;
} KeyboardSource;

#endif