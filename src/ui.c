#include "../headers/ui.h"
#include "../headers/events.h"
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

        gchar *button_markup = g_strdup_printf(
			"%s\n<span size='small' alpha='50%%'>%d</span>",
			display_accent, i + 1
		);
		GtkWidget *button = gtk_button_new();
		GtkWidget *label = gtk_label_new(NULL);
		gtk_label_set_markup(GTK_LABEL(label), button_markup);
		gtk_label_set_xalign(GTK_LABEL(label), 0.5);
		gtk_label_set_yalign(GTK_LABEL(label), 0.5);
		gtk_button_set_child(GTK_BUTTON(button), label);
		gtk_widget_set_size_request(button, -1, -1);

        gtk_widget_add_css_class(button, "eeeeee-button");

        g_free(button_markup);

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

void setup_popup_styling() {
    GtkCssProvider *css_provider = gtk_css_provider_new();

    const char *css_data =
        ".eeeeee-popup {\n"
        "    background-color: #fff;\n"
        "    border-radius: 16px;\n"
        "    box-shadow: 0 2px 6px rgba(0, 0, 0, 0.2);\n"
		"    padding: 8px 12px;\n"
        "}\n"
        ".eeeeee-button {\n"
        "    background: none;\n"
        "    border: none;\n"
        "    border-radius: 12px;\n"
		"    padding: 24px;\n"
        "    transition: background-color 0.2s ease;\n"
        "}\n"
        ".eeeeee-button label {\n"
        "    color: #000;\n"
        "    font-size: 22px;\n"
        "    font-weight: normal;\n"
        "}\n"
        ".eeeeee-button:hover {\n"
        "    background-color: rgba(0, 0, 0, 0.05);\n"
        "    transform: translateY(-1px);\n"
        "}\n"
        ".eeeeee-button:active {\n"
        "    background-color: rgba(0, 0, 0, 0.1);\n"
        "    transform: translateY(0);\n"
        "    box-shadow: none;\n"
        "}\n";

    gtk_css_provider_load_from_string(css_provider, css_data);

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
    gtk_window_set_default_size(GTK_WINDOW(app->popup_window), 300, 40);

    gtk_widget_add_css_class(app->popup_window, "eeeeee-popup");

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(app->popup_window), main_box);

    // Create and store reference to button box
    app->popup_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_halign(app->popup_button_box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(app->popup_button_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(app->popup_button_box, 10);
    gtk_widget_set_margin_end(app->popup_button_box, 10);
    gtk_widget_set_margin_bottom(app->popup_button_box, 10);
    gtk_box_append(GTK_BOX(main_box), app->popup_button_box);

	setup_popup_styling();
}

void on_activate(GtkApplication *gtk_app, gpointer user_data) {
    AppData *app = (AppData *)user_data;

    app->window = gtk_application_window_new(gtk_app);

    create_popup_window(app);

    setup_keyboard_monitoring(app);
}