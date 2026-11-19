#ifndef INPUT_H
#define INPUT_H

#include "types.h"

int setup_virtual_keyboard(void);
void cleanup_virtual_keyboard(void);
void type_accent_char(const char *accent);

#endif