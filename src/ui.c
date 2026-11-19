#include "../headers/ui.h"
#include "../headers/events.h"
#include "../headers/keyboard.h"
#include "../headers/config.h"
#include "../headers/input.h"
#include <ctype.h>

typedef struct {
    const char *letter_name;
    const char *accents[10]; // Adjust size as needed
} AccentData;

static AccentData get_accent_data_for_key(int key_code) {
    AccentData data = {0};

    switch (key_code) {
        case KEY_A:
            data.letter_name = "a";
            data.accents[0] = "á";
            data.accents[1] = "à";
            data.accents[2] = "â";
            data.accents[3] = "ä";
            data.accents[4] = "ā";
            data.accents[5] = "ã";
            data.accents[6] = NULL;
            break;
        case KEY_E:
            data.letter_name = "e";
            data.accents[0] = "é";
            data.accents[1] = "è";
            data.accents[2] = "ê";
            data.accents[3] = "ë";
            data.accents[4] = "ē";
            data.accents[5] = NULL;
            break;
        case KEY_I:
            data.letter_name = "i";
            data.accents[0] = "í";
            data.accents[1] = "ì";
            data.accents[2] = "î";
            data.accents[3] = "ï";
            data.accents[4] = "ī";
            data.accents[5] = NULL;
            break;
        case KEY_O:
            data.letter_name = "o";
            data.accents[0] = "ó";
            data.accents[1] = "ò";
            data.accents[2] = "ô";
            data.accents[3] = "ö";
            data.accents[4] = "ō";
            data.accents[5] = "õ";
            data.accents[6] = NULL;
            break;
        case KEY_U:
            data.letter_name = "u";
            data.accents[0] = "ú";
            data.accents[1] = "ù";
            data.accents[2] = "û";
            data.accents[3] = "ü";
            data.accents[4] = "ū";
            data.accents[5] = NULL;
            break;
        case KEY_Y:
            data.letter_name = "y";
            data.accents[0] = "ý";
            data.accents[1] = "ÿ";
            data.accents[2] = NULL;
            break;
        case KEY_C:
            data.letter_name = "c";
            data.accents[0] = "ç";
            data.accents[1] = "ć";
            data.accents[2] = "č";
            data.accents[3] = NULL;
            break;
        default:
            data.letter_name = "unknown";
            data.accents[0] = NULL;
            break;
    }
    return data;
}

static gboolean type_accent_delayed(gpointer user_data) {
    char *accent = (char *)user_data;
    type_accent_char(accent);
    g_free(accent);
    return G_SOURCE_REMOVE;
}

void on_accent_selected(GtkWidget *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    const char *accent = (const char *)g_object_get_data(G_OBJECT(button), "accent");

    if (!accent) return;

    printf("Selected accent: %s\n", accent);

    hide_popup(app);

    char *accent_copy = g_strdup(accent);
    g_timeout_add(100, type_accent_delayed, accent_copy);
}

void update_popup_content(AppData *app, int key_code) {
    if (!app->popup_button_box) return;

    AccentData accent_data = get_accent_data_for_key(key_code);

    // Clear existing buttons
    GtkWidget *child = gtk_widget_get_first_child(app->popup_button_box);
    while (child) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(app->popup_button_box), child);
        child = next;
    }

    for (int i = 0; accent_data.accents[i] != NULL; i++) {
        const char *accent = accent_data.accents[i];

        const char *display_accent;
        gchar *upper_accent = NULL;

        if (app->shift_key_hold) {
            upper_accent = g_utf8_strup(accent, -1);
            display_accent = upper_accent;
        } else {
            display_accent = accent;
        }

        gchar *number = g_strdup_printf("%d", i + 1);

        GtkWidget *cell = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkWidget *char_label = gtk_label_new(display_accent);
        GtkWidget *number_label = gtk_label_new(number);
        gtk_widget_add_css_class(char_label, "eeeeee-char");
        gtk_widget_add_css_class(number_label, "eeeeee-number");
        gtk_box_append(GTK_BOX(cell), char_label);
        gtk_box_append(GTK_BOX(cell), number_label);

        GtkWidget *button = gtk_button_new();
        gtk_button_set_child(GTK_BUTTON(button), cell);
        gtk_widget_add_css_class(button, "eeeeee-button");

        g_free(number);

        g_object_set_data_full(G_OBJECT(button), "accent",
                              g_strdup(display_accent), g_free);

        g_signal_connect(button, "clicked",
                        G_CALLBACK(on_accent_selected), app);

        gtk_box_append(GTK_BOX(app->popup_button_box), button);

        if (upper_accent) {
            g_free(upper_accent);
        }
    }
}

void setup_popup_styling(void) {
    GtkCssProvider *css_provider = gtk_css_provider_new();

    // Modeled on the macOS press-and-hold accent menu
    const char *css_data =
        ".eeeeee-popup {\n"
        "    background-color: #ececec;\n"
        "    border: 1px solid rgba(0, 0, 0, 0.15);\n"
        "    border-radius: 12px;\n"
        "    padding: 4px;\n"
        "}\n"
        ".eeeeee-button {\n"
        "    background: none;\n"
        "    border: none;\n"
        "    box-shadow: none;\n"
        "    outline: none;\n"
        "    border-radius: 7px;\n"
        "    min-width: 30px;\n"
        "    min-height: 0;\n"
        "    padding: 3px 6px 2px 6px;\n"
        "}\n"
        ".eeeeee-char {\n"
        "    color: #1d1d1f;\n"
        "    font-size: 20px;\n"
        "}\n"
        ".eeeeee-number {\n"
        "    color: #8e8e93;\n"
        "    font-size: 11px;\n"
        "}\n"
        ".eeeeee-button:hover,\n"
        ".eeeeee-button:focus-visible {\n"
        "    background-color: #007aff;\n"
        "}\n"
        ".eeeeee-button:hover .eeeeee-char,\n"
        ".eeeeee-button:hover .eeeeee-number,\n"
        ".eeeeee-button:focus-visible .eeeeee-char,\n"
        ".eeeeee-button:focus-visible .eeeeee-number {\n"
        "    color: #ffffff;\n"
        "}\n"
        "@media (prefers-color-scheme: dark) {\n"
        "    .eeeeee-popup {\n"
        "        background-color: #2c2c2e;\n"
        "        border-color: rgba(255, 255, 255, 0.15);\n"
        "    }\n"
        "    .eeeeee-char { color: #f5f5f7; }\n"
        "    .eeeeee-number { color: #98989d; }\n"
        "    .eeeeee-button:hover,\n"
        "    .eeeeee-button:focus-visible { background-color: #0a84ff; }\n"
        "}\n";

    gtk_css_provider_load_from_string(css_provider, css_data);

    // Make the dark @media query follow the system light/dark preference
    g_object_bind_property(gtk_settings_get_default(), "gtk-interface-color-scheme",
                           css_provider, "prefers-color-scheme",
                           G_BINDING_SYNC_CREATE);

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    g_object_unref(css_provider);
}

void create_popup_window(AppData *app) {
    app->popup_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(app->popup_window), "eeeeee");
    gtk_window_set_decorated(GTK_WINDOW(app->popup_window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(app->popup_window), FALSE);
    gtk_widget_add_css_class(app->popup_window, "eeeeee-popup");

    // Create and store reference to button box
    app->popup_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_window_set_child(GTK_WINDOW(app->popup_window), app->popup_button_box);

	setup_popup_styling();
}

void on_activate(GtkApplication *gtk_app, gpointer user_data) {
    AppData *app = (AppData *)user_data;

	if (app->window) {
		return;
	}

    app->window = gtk_application_window_new(gtk_app);

    create_popup_window(app);

    app->configured_time = load_hold_duration();
    printf("Hold duration: %ims\n", app->configured_time);

    if (!setup_keyboard_monitoring(app)) {
        g_application_quit(G_APPLICATION(gtk_app));
    }
}