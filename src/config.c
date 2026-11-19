#define _POSIX_C_SOURCE 200809L
#include "../headers/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_config_file_path(void) {
    const char *home = getenv("HOME");
    if (!home) return NULL;

    return g_strdup_printf("%s/%s/%s", home, CONFIG_DIR, CONFIG_FILE);
}

int load_hold_duration(void) {
    int hold_duration = DEFAULT_HOLD_DURATION_MS;

    char *config_file = get_config_file_path();
    if (!config_file) return hold_duration;

    FILE *f = fopen(config_file, "r");
    g_free(config_file);
    if (!f) return hold_duration;

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "hold_duration=", 14) == 0) {
            int duration = atoi(line + 14);
            if (duration > 0) {
                hold_duration = duration;
            }
        }
    }

    fclose(f);
    return hold_duration;
}
