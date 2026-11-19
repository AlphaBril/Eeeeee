#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

bool setup_keyboard_monitoring(AppData *app);
void remove_keyboard_device(AppData *app, KeyboardSource *kbd);
void cleanup_keyboard_monitoring(AppData *app);

#endif
