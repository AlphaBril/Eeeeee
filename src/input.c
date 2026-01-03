#define _POSIX_C_SOURCE 200809L
#include "../headers/input.h"
#include <linux/uinput.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

static int uinput_fd = -1;

void usleep_custom(int time) {
    struct timespec ts = {0, time * 1000000L};
    nanosleep(&ts, NULL);
}

int setup_virtual_keyboard(void) {
    uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd < 0) {
        printf("Error opening /dev/uinput: %s\n", strerror(errno));
        return -1;
    }

    ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN);

    for (int i = 0; i < KEY_MAX; i++) {
        ioctl(uinput_fd, UI_SET_KEYBIT, i);
    }

    struct uinput_setup usetup = {};
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "Accent Popup Virtual Keyboard");

    ioctl(uinput_fd, UI_DEV_SETUP, &usetup);
    ioctl(uinput_fd, UI_DEV_CREATE);

    printf("✓ Virtual keyboard ready\n");
    return 0;
}

void cleanup_virtual_keyboard(void) {
    if (uinput_fd >= 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
        uinput_fd = -1;
    }
    printf("✓ Virtual keyboard cleanedup\n");
}
static void emit_event(int type, int code, int value) {
    struct input_event ie = {};
    ie.type = type;
    ie.code = code;
    ie.value = value;

    if (write(uinput_fd, &ie, sizeof(ie)) < 0) {
        printf("Error writing event: %s\n", strerror(errno));
    }
}

static void press_key(int key) {
    emit_event(EV_KEY, key, 1);  // Press
    emit_event(EV_SYN, SYN_REPORT, 0);
    usleep_custom(5);
    emit_event(EV_KEY, key, 0);  // Release
    emit_event(EV_SYN, SYN_REPORT, 0);
    usleep_custom(5);
}

static void type_unicode(const char* hex_code) {
    emit_event(EV_KEY, KEY_LEFTCTRL, 1);
    emit_event(EV_KEY, KEY_LEFTSHIFT, 1);
    emit_event(EV_KEY, KEY_U, 1);
    emit_event(EV_SYN, SYN_REPORT, 0);
    usleep_custom(5);

    emit_event(EV_KEY, KEY_U, 0);
    emit_event(EV_KEY, KEY_LEFTSHIFT, 0);
    emit_event(EV_KEY, KEY_LEFTCTRL, 0);
    emit_event(EV_SYN, SYN_REPORT, 0);
    usleep_custom(10);

    // Type hex code
    for (int i = 0; hex_code[i]; i++) {
        int key = -1;
        switch (hex_code[i]) {
            case '0': key = KEY_0; break;
            case '1': key = KEY_1; break;
            case '2': key = KEY_2; break;
            case '3': key = KEY_3; break;
            case '4': key = KEY_4; break;
            case '5': key = KEY_5; break;
            case '6': key = KEY_6; break;
            case '7': key = KEY_7; break;
            case '8': key = KEY_8; break;
            case '9': key = KEY_9; break;
            case 'a': case 'A': key = KEY_A; break;
            case 'b': case 'B': key = KEY_B; break;
            case 'c': case 'C': key = KEY_C; break;
            case 'd': case 'D': key = KEY_D; break;
            case 'e': case 'E': key = KEY_E; break;
            case 'f': case 'F': key = KEY_F; break;
        }
        if (key != -1) {
            press_key(key);
        }
    }

    press_key(KEY_ENTER);
}

typedef struct {
    const char* accent;
    const char* unicode;
} accent_mapping_t;

static const accent_mapping_t accent_map[] = {
    // A accents
    {"á", "e1"}, {"à", "e0"}, {"â", "e2"}, {"ä", "e4"}, {"ā", "101"}, {"ã", "e3"},
    {"Á", "c1"}, {"À", "c0"}, {"Â", "c2"}, {"Ä", "c4"}, {"Ā", "100"}, {"Ã", "c3"},

    // E accents
    {"é", "e9"}, {"è", "e8"}, {"ê", "ea"}, {"ë", "eb"}, {"ē", "113"},
    {"É", "c9"}, {"È", "c8"}, {"Ê", "ca"}, {"Ë", "cb"}, {"Ē", "112"},

    // I accents
    {"í", "ed"}, {"ì", "ec"}, {"î", "ee"}, {"ï", "ef"}, {"ī", "12b"},
    {"Í", "cd"}, {"Ì", "cc"}, {"Î", "ce"}, {"Ï", "cf"}, {"Ī", "12a"},

    // O accents
    {"ó", "f3"}, {"ò", "f2"}, {"ô", "f4"}, {"ö", "f6"}, {"ō", "14d"}, {"õ", "f5"},
    {"Ó", "d3"}, {"Ò", "d2"}, {"Ô", "d4"}, {"Ö", "d6"}, {"Ō", "14c"}, {"Õ", "d5"},

    // U accents
    {"ú", "fa"}, {"ù", "f9"}, {"û", "fb"}, {"ü", "fc"}, {"ū", "16b"},
    {"Ú", "da"}, {"Ù", "d9"}, {"Û", "db"}, {"Ü", "dc"}, {"Ū", "16a"},

    // Y accents
    {"ý", "fd"}, {"ÿ", "ff"},
    {"Ý", "dd"}, {"Ÿ", "178"},

    // C accents
    {"ç", "e7"}, {"ć", "107"}, {"č", "10d"},
    {"Ç", "c7"}, {"Ć", "106"}, {"Č", "10c"},

    {NULL, NULL}
};

void type_accent_char(const char *accent) {
    if (uinput_fd < 0) {
        printf("Virtual keyboard not available\n");
        return;
    }

    printf("Removing key pressed\n");
    emit_event(EV_KEY, KEY_BACKSPACE, 1);
    emit_event(EV_SYN, SYN_REPORT, 0);
    usleep_custom(5);
    emit_event(EV_KEY, KEY_BACKSPACE, 0);
    emit_event(EV_SYN, SYN_REPORT, 0);
    usleep_custom(10);

    printf("Typing accent: %s\n", accent);

    for (int i = 0; accent_map[i].accent != NULL; i++) {
        if (strcmp(accent, accent_map[i].accent) == 0) {
            type_unicode(accent_map[i].unicode);
            return;
        }
    }

    printf("Unknown accent: %s\n", accent);

    unsigned char first_byte = (unsigned char)accent[0];
    char base_char = 'e'; // default fallback

    if (first_byte >= 0xC0 && first_byte <= 0xC6) base_char = 'A';
    else if (first_byte >= 0xC8 && first_byte <= 0xCB) base_char = 'E';
    else if (first_byte >= 0xCC && first_byte <= 0xCF) base_char = 'I';
    else if (first_byte >= 0xD2 && first_byte <= 0xD6) base_char = 'O';
    else if (first_byte >= 0xD9 && first_byte <= 0xDC) base_char = 'U';
    else if (first_byte >= 0xE0 && first_byte <= 0xE6) base_char = 'a';
    else if (first_byte >= 0xE8 && first_byte <= 0xEB) base_char = 'e';
    else if (first_byte >= 0xEC && first_byte <= 0xEF) base_char = 'i';
    else if (first_byte >= 0xF2 && first_byte <= 0xF6) base_char = 'o';
    else if (first_byte >= 0xF9 && first_byte <= 0xFC) base_char = 'u';
    else if (first_byte == 0xC7 || first_byte == 0xE7) base_char = (first_byte == 0xC7) ? 'C' : 'c';

    switch(base_char) {
        case 'a': press_key(KEY_A); break;
        case 'A': press_key(KEY_A); break;
        case 'e': press_key(KEY_E); break;
        case 'E': press_key(KEY_E); break;
        case 'i': press_key(KEY_I); break;
        case 'I': press_key(KEY_I); break;
        case 'o': press_key(KEY_O); break;
        case 'O': press_key(KEY_O); break;
        case 'u': press_key(KEY_U); break;
        case 'U': press_key(KEY_U); break;
        case 'y': press_key(KEY_Y); break;
        case 'Y': press_key(KEY_Y); break;
        case 'c': press_key(KEY_C); break;
        case 'C': press_key(KEY_C); break;
        default: press_key(KEY_E); break;
    }
}