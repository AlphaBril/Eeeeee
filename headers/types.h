#ifndef TYPES_H
#define TYPES_H

#include <gtk/gtk.h>
#include <libevdev/libevdev.h>
#include <sys/time.h>
#include <stdbool.h>

#define DEFAULT_HOLD_DURATION_MS 500
#define CONFIG_DIR ".config/eeeeee"
#define CONFIG_FILE "config"

typedef struct {
    GtkWidget *window;
    GtkWidget *popup_window;
    GtkWidget *label;
	GtkWidget *popup_label;
    GtkWidget *popup_button_box;

    int kbd_fd;
    struct libevdev *dev;
    int key_pressed;
	bool shift_key_hold;
	const char **current_accents;
    struct timeval press_time;
    guint timer_id;
    bool popup_shown;

    char *keyboard_device;
	int configured_time;
} AppData;

typedef struct {
    char *device_path;
    char *device_name;
    bool has_full_keyboard;
} KeyboardDevice;

#endif