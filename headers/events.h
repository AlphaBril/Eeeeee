#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"
#include <glib.h>

gint64 get_time_ms(void);
void hide_popup(AppData *app);
void show_popup(AppData *app);
gboolean check_hold_timer(gpointer user_data);
void handle_key_event(AppData *app, int code, int value);
gboolean keyboard_event_handler(GIOChannel *channel, GIOCondition condition, gpointer user_data);
void setup_keyboard_monitoring(AppData *app);

#endif