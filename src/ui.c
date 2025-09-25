#include "../headers/ui.h"
#include "../headers/events.h"
#include "../headers/input.h"

// Callback function for typing the accent after delay
static gboolean type_accent_delayed(gpointer user_data) {
    char *accent = (char *)user_data;
    type_accent_char(accent);
    g_free(accent); // Free the duplicated string
    return G_SOURCE_REMOVE;
}

void on_accent_selected(GtkWidget *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    const char *accent = (const char *)g_object_get_data(G_OBJECT(button), "accent");

    if (!accent) return;

    printf("Selected accent: %s\n", accent);

    // Hide popup first
    hide_popup(app);

    // Small delay then type the accent
    // Duplicate the string since we need it in the callback
    char *accent_copy = g_strdup(accent);
    g_timeout_add(100, type_accent_delayed, accent_copy);
}

void create_popup_window(AppData *app) {
    app->popup_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(app->popup_window), "Select Accent");
    gtk_window_set_decorated(GTK_WINDOW(app->popup_window), FALSE);  // No decoration for quick access
    gtk_window_set_resizable(GTK_WINDOW(app->popup_window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(app->popup_window), 300, 100);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(app->popup_window), main_box);

    GtkWidget *label = gtk_label_new("Select accent for 'e':");
    gtk_widget_set_margin_start(label, 10);
    gtk_widget_set_margin_end(label, 10);
    gtk_widget_set_margin_top(label, 10);
    gtk_box_append(GTK_BOX(main_box), label);

    // Create button box for accents
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(button_box, 10);
    gtk_widget_set_margin_end(button_box, 10);
    gtk_widget_set_margin_bottom(button_box, 10);
    gtk_box_append(GTK_BOX(main_box), button_box);

    // Accent options
    const char *accents[] = {"é", "è", "ê", "ë", "ē", NULL};

    for (int i = 0; accents[i] != NULL; i++) {
        GtkWidget *button = gtk_button_new_with_label(accents[i]);
        gtk_widget_set_size_request(button, 45, 35);

        // Store the accent character with the button
        g_object_set_data_full(G_OBJECT(button), "accent",
                              g_strdup(accents[i]), g_free);

        g_signal_connect(button, "clicked",
                        G_CALLBACK(on_accent_selected), app);

        gtk_box_append(GTK_BOX(button_box), button);
    }
}

void on_activate(GtkApplication *gtk_app, gpointer user_data) {
    AppData *app = (AppData *)user_data;

    app->window = gtk_application_window_new(gtk_app);

    create_popup_window(app);

    setup_keyboard_monitoring(app);
}