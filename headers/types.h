#ifndef TYPES_H
#define TYPES_H

#include <gtk/gtk.h>
#include <libevdev/libevdev.h>
#include <sys/time.h>
#include <stdbool.h>

#define HOLD_DURATION_MS 2000
#define CONFIG_DIR ".config/accent-popup"
#define CONFIG_FILE "config"

typedef struct {
    GtkWidget *window;
    GtkWidget *popup_window;
    GtkWidget *label;

    int kbd_fd;
    struct libevdev *dev;
    bool e_key_pressed;
    struct timeval press_time;
    guint timer_id;
    bool popup_shown;

    char *keyboard_device;
} AppData;

typedef struct {
    char *device_path;
    char *device_name;
    bool has_full_keyboard;
} KeyboardDevice;

#endif