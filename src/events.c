#define _POSIX_C_SOURCE 200809L
#include "../headers/events.h"
#include "../headers/keyboard.h"
#include "../headers/ui.h"
#include <stdio.h>
#include <sys/time.h>
#include <linux/input.h>
#include <gio/gio.h>
#include <unistd.h>

gint64 get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (gint64)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void select_accent_by_number(AppData *app, int number) {
    if (!app->popup_shown || !app->popup_button_box) return;

    // Get the nth button (number-1 because we start from 1)
    GtkWidget *child = gtk_widget_get_first_child(app->popup_button_box);
    int index = 0;

    while (child && index < (number - 1)) {
        child = gtk_widget_get_next_sibling(child);
        index++;
    }

    if (child && GTK_IS_BUTTON(child)) {
        // Simulate button click
        g_signal_emit_by_name(child, "clicked");
    }
}

void hide_popup(AppData *app) {
    if (app->popup_shown && app->popup_window) {
        gtk_widget_set_visible(app->popup_window, FALSE);
        app->popup_shown = false;
        printf("Popup hidden\n");
    }
}

void show_popup(AppData *app) {
    if (!app->popup_shown && app->popup_window) {
        update_popup_content(app, app->key_pressed);
        gtk_widget_set_visible(app->popup_window, TRUE);
        gtk_window_present(GTK_WINDOW(app->popup_window));
        // Like macOS: nothing is highlighted until the arrow keys are used
        gtk_window_set_focus_visible(GTK_WINDOW(app->popup_window), FALSE);
        app->popup_shown = true;
        printf("Popup shown after holding '%i' for %ims\n", app->key_pressed, app->configured_time);
    }
}

gboolean check_hold_timer(gpointer user_data) {
    AppData *app = (AppData *)user_data;

    if (app->key_pressed) {
        gint64 current_time = get_time_ms();
        gint64 press_time_ms = (gint64)app->press_time.tv_sec * 1000 + app->press_time.tv_usec / 1000;
        gint64 elapsed = current_time - press_time_ms;

        if (elapsed >= app->configured_time) {
            show_popup(app);
            app->timer_id = 0;
            return G_SOURCE_REMOVE;
        }
    } else {
        app->timer_id = 0;
        return G_SOURCE_REMOVE;
    }

    return G_SOURCE_CONTINUE;
}

bool is_key_listened(int code) {
	switch (code) {
        case KEY_A:
        case KEY_C:
        case KEY_E:
        case KEY_I:
        case KEY_O:
        case KEY_U:
        case KEY_Y:
        case KEY_LEFTSHIFT:
            return true;
        case KEY_1:
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
            return true;
        case KEY_ESC:
            return true;
        default:
            return false;
	}
}

void handle_key_event(AppData *app, int code, int value) {
    if (is_key_listened(code)) {
        if (code == KEY_LEFTSHIFT) {
            if (value == 1) {
                if (!app->shift_key_hold) {
                    app->shift_key_hold = true;
                }
            } else if (value == 0) {
                app->shift_key_hold = false;
            }
        } else if (code == KEY_ESC) {
            if (value == 1 && app->popup_shown) {
                hide_popup(app);
            }
        } else if (code >= KEY_1 && code <= KEY_9) {
            if (value == 1 && app->popup_shown) {
                int number = code - KEY_1 + 1;
                select_accent_by_number(app, number);
            }
        } else {
            if (value == 1) {
                if (!app->key_pressed) {
                    app->key_pressed = code;
                    gettimeofday(&app->press_time, NULL);

                    if (app->timer_id > 0) {
                        g_source_remove(app->timer_id);
                    }
                    app->timer_id = g_timeout_add(100, check_hold_timer, app);
                }
            } else if (value == 0) {
                app->key_pressed = 0;

                if (app->timer_id > 0) {
                    g_source_remove(app->timer_id);
                    app->timer_id = 0;
                }
            }
        }
    }
}

gboolean keyboard_event_handler(
	GIOChannel *channel __attribute__((unused)),
    GIOCondition condition __attribute__((unused)),
    gpointer user_data
) {
    KeyboardSource *kbd = (KeyboardSource *)user_data;
    AppData *app = kbd->app;
    unsigned int flags = LIBEVDEV_READ_FLAG_NORMAL;
    struct input_event ev;

    for (;;) {
        int rc = libevdev_next_event(kbd->dev, flags, &ev);

        if (rc == LIBEVDEV_READ_STATUS_SUCCESS || rc == LIBEVDEV_READ_STATUS_SYNC) {
            // Dropped events: replay the device state before going back to normal reads
            if (rc == LIBEVDEV_READ_STATUS_SYNC) {
                flags = LIBEVDEV_READ_FLAG_SYNC;
            }
            if (ev.type == EV_KEY) {
                handle_key_event(app, ev.code, ev.value);
            }
        } else if (rc == -EAGAIN) {
            if (flags == LIBEVDEV_READ_FLAG_SYNC) {
                flags = LIBEVDEV_READ_FLAG_NORMAL;
                continue;
            }
            return G_SOURCE_CONTINUE;
        } else {
            // Device unplugged (-ENODEV) or read error: stop watching it
            kbd->watch_id = 0;
            remove_keyboard_device(app, kbd);
            return G_SOURCE_REMOVE;
        }
    }
}