#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

char* get_config_dir(void);
char* get_config_file_path(void);
void save_keyboard_config(const char *device_path);
char* load_keyboard_config(int *configured_time);
void setup_keyboard_interactive(void);

#endif