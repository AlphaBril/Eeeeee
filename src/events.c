#define _POSIX_C_SOURCE 200809L
#include "../headers/events.h"
#include "../headers/keyboard.h"
#include <stdio.h>
#include <sys/time.h>
#include <linux/input.h>
#include <gio/gio.h>

gint64 get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (gint64)tv.tv_sec * 1000 + tv.tv_usec / 1000;
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
        gtk_widget_set_visible(app->popup_window, TRUE);
        gtk_window_present(GTK_WINDOW(app->popup_window));
        app->popup_shown = true;
        printf("Popup shown after holding '%i' for %i seconds\n", app->key_pressed, app->configured_time);
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
			return true;
		case KEY_C:
			return true;
		case KEY_E:
			return true;
		case KEY_I:
			return true;
		case KEY_O:
			return true;
		case KEY_U:
			return true;
		case KEY_Y:
			return true;
		case KEY_LEFTSHIFT:
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
    AppData *app = (AppData *)user_data;
    struct input_event ev;
    int rc;

    do {
        rc = libevdev_next_event(app->dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == 0) {
            if (ev.type == EV_KEY) {
                handle_key_event(app, ev.code, ev.value);
            }
        } else if (rc == -EAGAIN) {
            break;
        }
    } while (rc == 0);

    return G_SOURCE_CONTINUE;
}

void setup_keyboard_monitoring(AppData *app) {
    app->kbd_fd = find_keyboard_device(app);
    if (app->kbd_fd < 0) {
        return;
    }

    int rc = libevdev_new_from_fd(app->kbd_fd, &app->dev);
    if (rc < 0) {
        close(app->kbd_fd);
        return;
    }

    GIOChannel *kbd_channel = g_io_channel_unix_new(app->kbd_fd);
    g_io_add_watch(kbd_channel, G_IO_IN, keyboard_event_handler, app);
    g_io_channel_unref(kbd_channel);

    printf("✓ Keyboard monitoring active, waiting for input\n");
}