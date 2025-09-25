#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

void free_keyboard_devices(KeyboardDevice *devices, int count);
KeyboardDevice* scan_keyboard_devices(int *count);
int find_keyboard_device(AppData *app);

#endif