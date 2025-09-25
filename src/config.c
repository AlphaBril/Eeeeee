#define _POSIX_C_SOURCE 200809L
#include "../headers/config.h"
#include "../headers/keyboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <linux/input.h>

char* get_config_dir(void) {
    const char *home = getenv("HOME");
    if (!home) return NULL;

    char *config_dir = malloc(strlen(home) + strlen(CONFIG_DIR) + 2);
    sprintf(config_dir, "%s/%s", home, CONFIG_DIR);
    return config_dir;
}

char* get_config_file_path(void) {
    char *config_dir = get_config_dir();
    if (!config_dir) return NULL;

    char *config_file = malloc(strlen(config_dir) + strlen(CONFIG_FILE) + 2);
    sprintf(config_file, "%s/%s", config_dir, CONFIG_FILE);
    free(config_dir);
    return config_file;
}

void save_keyboard_config(const char *device_path) {
    char *config_dir = get_config_dir();
    char *config_file = get_config_file_path();

    if (!config_dir || !config_file) {
        printf("Error: Could not determine config file path\n");
        return;
    }

    mkdir(config_dir, 0755);

    FILE *f = fopen(config_file, "w");
    if (f) {
        fprintf(f, "keyboard_device=%s\n", device_path);
        fclose(f);
        printf("Configuration saved to: %s\n", config_file);
    } else {
        printf("Error: Could not save configuration to %s: %s\n", config_file, strerror(errno));
    }

    free(config_dir);
    free(config_file);
}

char* load_keyboard_config(void) {
    char *config_file = get_config_file_path();
    if (!config_file) return NULL;

    FILE *f = fopen(config_file, "r");
    if (!f) {
        free(config_file);
        return NULL;
    }

    char line[512];
    char *device_path = NULL;

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "keyboard_device=", 16) == 0) {
            char *newline = strchr(line + 16, '\n');
            if (newline) *newline = '\0';
            device_path = strdup(line + 16);
            break;
        }
    }

    fclose(f);
    free(config_file);
    return device_path;
}

void setup_keyboard_interactive(void) {
    int device_count;
    KeyboardDevice *devices = scan_keyboard_devices(&device_count);

    if (!devices || device_count == 0) {
        printf("No keyboard devices found!\n");
        printf("Make sure you're in the 'input' group: sudo usermod -a -G input $USER\n");
        return;
    }

    printf("\n=== Accent Popup Keyboard Setup ===\n");
    printf("Found %d keyboard device(s):\n\n", device_count);

    for (int i = 0; i < device_count; i++) {
        printf("%d. %s\n", i + 1, devices[i].device_path);
        printf("   Name: %s\n", devices[i].device_name);
        printf("   Type: %s\n", devices[i].has_full_keyboard ? "Full keyboard" : "Partial keyboard");
        printf("\n");
    }

    printf("Which keyboard do you want to use for accent popup? (1-%d): ", device_count);
    fflush(stdout);

    int choice;
    if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= device_count) {
        const char *selected_device = devices[choice - 1].device_path;
        printf("\nSelected: %s (%s)\n", selected_device, devices[choice - 1].device_name);

        printf("Testing device... Press 'e' key to confirm it works, or Ctrl+C to cancel.\n");

        int fd = open(selected_device, O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            struct libevdev *dev;
            if (libevdev_new_from_fd(fd, &dev) >= 0) {
                struct input_event ev;
                bool test_passed = false;

                for (int timeout = 0; timeout < 100 && !test_passed; timeout++) {
                    while (libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev) == 0) {
                        if (ev.type == EV_KEY && ev.code == KEY_E && ev.value == 1) {
                            printf("✓ Key press detected! Device is working.\n");
                            test_passed = true;
                            break;
                        }
                    }
                    struct timespec ts = {0, 100000000L};
                    nanosleep(&ts, NULL);
                }

                if (test_passed) {
                    save_keyboard_config(selected_device);
                    printf("✓ Configuration saved successfully!\n");
                    printf("You can now run 'accent-popup' to use the program.\n");
                } else {
                    printf("⚠ No key press detected. The device might not be working properly.\n");
                    printf("Save configuration anyway? (y/n): ");
                    char confirm;
                    if (scanf(" %c", &confirm) != 1) {
    					confirm = 'n';  // Default to 'n' if input fails
					}
                    if (confirm == 'y' || confirm == 'Y') {
                        save_keyboard_config(selected_device);
                        printf("Configuration saved.\n");
                    }
                }

                libevdev_free(dev);
            }
            close(fd);
        }
    } else {
        printf("Invalid choice.\n");
    }

    free_keyboard_devices(devices, device_count);
}