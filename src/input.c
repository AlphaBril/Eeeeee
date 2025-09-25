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

    // Enable key events
    ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN);

    // Enable all keyboard keys
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

void type_accent_char(const char *accent) {
    if (uinput_fd < 0) {
        printf("Virtual keyboard not available\n");
        return;
    }

    printf("Typing accent: %s\n", accent);

    // E accents (lowercase)
    if (strcmp(accent, "é") == 0) {
        type_unicode("e9");  // Unicode for é
    } else if (strcmp(accent, "è") == 0) {
        type_unicode("e8");  // Unicode for è
    } else if (strcmp(accent, "ê") == 0) {
        type_unicode("ea");  // Unicode for ê
    } else if (strcmp(accent, "ë") == 0) {
        type_unicode("eb");  // Unicode for ë
    } else if (strcmp(accent, "ē") == 0) {
        type_unicode("113"); // Unicode for ē

    // E accents (uppercase)
    } else if (strcmp(accent, "É") == 0) {
        type_unicode("c9");  // Unicode for É
    } else if (strcmp(accent, "È") == 0) {
        type_unicode("c8");  // Unicode for È
    } else if (strcmp(accent, "Ê") == 0) {
        type_unicode("ca");  // Unicode for Ê
    } else if (strcmp(accent, "Ë") == 0) {
        type_unicode("cb");  // Unicode for Ë
    } else if (strcmp(accent, "Ē") == 0) {
        type_unicode("112"); // Unicode for Ē

    // A accents (lowercase)
    } else if (strcmp(accent, "á") == 0) {
        type_unicode("e1");  // Unicode for á
    } else if (strcmp(accent, "à") == 0) {
        type_unicode("e0");  // Unicode for à
    } else if (strcmp(accent, "â") == 0) {
        type_unicode("e2");  // Unicode for â
    } else if (strcmp(accent, "ä") == 0) {
        type_unicode("e4");  // Unicode for ä
    } else if (strcmp(accent, "ā") == 0) {
        type_unicode("101"); // Unicode for ā

    // A accents (uppercase)
    } else if (strcmp(accent, "Á") == 0) {
        type_unicode("c1");  // Unicode for Á
    } else if (strcmp(accent, "À") == 0) {
        type_unicode("c0");  // Unicode for À
    } else if (strcmp(accent, "Â") == 0) {
        type_unicode("c2");  // Unicode for Â
    } else if (strcmp(accent, "Ä") == 0) {
        type_unicode("c4");  // Unicode for Ä
    } else if (strcmp(accent, "Ā") == 0) {
        type_unicode("100"); // Unicode for Ā

    // I accents (lowercase)
    } else if (strcmp(accent, "í") == 0) {
        type_unicode("ed");  // Unicode for í
    } else if (strcmp(accent, "ì") == 0) {
        type_unicode("ec");  // Unicode for ì
    } else if (strcmp(accent, "î") == 0) {
        type_unicode("ee");  // Unicode for î
    } else if (strcmp(accent, "ï") == 0) {
        type_unicode("ef");  // Unicode for ï
    } else if (strcmp(accent, "ī") == 0) {
        type_unicode("12b"); // Unicode for ī

    // I accents (uppercase)
    } else if (strcmp(accent, "Í") == 0) {
        type_unicode("cd");  // Unicode for Í
    } else if (strcmp(accent, "Ì") == 0) {
        type_unicode("cc");  // Unicode for Ì
    } else if (strcmp(accent, "Î") == 0) {
        type_unicode("ce");  // Unicode for Î
    } else if (strcmp(accent, "Ï") == 0) {
        type_unicode("cf");  // Unicode for Ï
    } else if (strcmp(accent, "Ī") == 0) {
        type_unicode("12a"); // Unicode for Ī

    // O accents (lowercase)
    } else if (strcmp(accent, "ó") == 0) {
        type_unicode("f3");  // Unicode for ó
    } else if (strcmp(accent, "ò") == 0) {
        type_unicode("f2");  // Unicode for ò
    } else if (strcmp(accent, "ô") == 0) {
        type_unicode("f4");  // Unicode for ô
    } else if (strcmp(accent, "ö") == 0) {
        type_unicode("f6");  // Unicode for ö
    } else if (strcmp(accent, "ō") == 0) {
        type_unicode("14d"); // Unicode for ō

    // O accents (uppercase)
    } else if (strcmp(accent, "Ó") == 0) {
        type_unicode("d3");  // Unicode for Ó
    } else if (strcmp(accent, "Ò") == 0) {
        type_unicode("d2");  // Unicode for Ò
    } else if (strcmp(accent, "Ô") == 0) {
        type_unicode("d4");  // Unicode for Ô
    } else if (strcmp(accent, "Ö") == 0) {
        type_unicode("d6");  // Unicode for Ö
    } else if (strcmp(accent, "Ō") == 0) {
        type_unicode("14c"); // Unicode for Ō

    // U accents (lowercase)
    } else if (strcmp(accent, "ú") == 0) {
        type_unicode("fa");  // Unicode for ú
    } else if (strcmp(accent, "ù") == 0) {
        type_unicode("f9");  // Unicode for ù
    } else if (strcmp(accent, "û") == 0) {
        type_unicode("fb");  // Unicode for û
    } else if (strcmp(accent, "ü") == 0) {
        type_unicode("fc");  // Unicode for ü
    } else if (strcmp(accent, "ū") == 0) {
        type_unicode("16b"); // Unicode for ū

    // U accents (uppercase)
    } else if (strcmp(accent, "Ú") == 0) {
        type_unicode("da");  // Unicode for Ú
    } else if (strcmp(accent, "Ù") == 0) {
        type_unicode("d9");  // Unicode for Ù
    } else if (strcmp(accent, "Û") == 0) {
        type_unicode("db");  // Unicode for Û
    } else if (strcmp(accent, "Ü") == 0) {
        type_unicode("dc");  // Unicode for Ü
    } else if (strcmp(accent, "Ū") == 0) {
        type_unicode("16a"); // Unicode for Ū

    // Y accents (lowercase)
    } else if (strcmp(accent, "ý") == 0) {
        type_unicode("fd");   // Unicode for ý
    } else if (strcmp(accent, "ỳ") == 0) {
        type_unicode("1ef3"); // Unicode for ỳ (grave)
    } else if (strcmp(accent, "ŷ") == 0) {
        type_unicode("177");  // Unicode for ŷ (circumflex)
    } else if (strcmp(accent, "ÿ") == 0) {
        type_unicode("ff");   // Unicode for ÿ (diaeresis)
    } else if (strcmp(accent, "ȳ") == 0) {
        type_unicode("233");  // Unicode for ȳ (macron)

    // Y accents (uppercase)
    } else if (strcmp(accent, "Ý") == 0) {
        type_unicode("dd");   // Unicode for Ý
    } else if (strcmp(accent, "Ỳ") == 0) {
        type_unicode("1ef2"); // Unicode for Ỳ (grave)
    } else if (strcmp(accent, "Ŷ") == 0) {
        type_unicode("176");  // Unicode for Ŷ (circumflex)
    } else if (strcmp(accent, "Ÿ") == 0) {
        type_unicode("178");  // Unicode for Ÿ (diaeresis)
    } else if (strcmp(accent, "Ȳ") == 0) {
        type_unicode("232");  // Unicode for Ȳ (macron)

    } else {
        printf("Unknown accent: %s\n", accent);
        // Fallback: just type the base character
        char base = accent[0];
        switch(base) {
            case 'a': case 'A': press_key(KEY_A); break;
            case 'e': case 'E': press_key(KEY_E); break;
            case 'i': case 'I': press_key(KEY_I); break;
            case 'o': case 'O': press_key(KEY_O); break;
            case 'u': case 'U': press_key(KEY_U); break;
            case 'y': case 'Y': press_key(KEY_Y); break;
            default: press_key(KEY_E); break;
        }
    }
}