#include "../headers/ui.h"
#include "../headers/events.h"

void create_popup_window(AppData *app) {
    app->popup_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(app->popup_window), "Accent Popup");
    gtk_window_set_decorated(GTK_WINDOW(app->popup_window), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(app->popup_window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(app->popup_window), 300, 100);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(app->popup_window), box);

    GtkWidget *popup_label = gtk_label_new("É È Ê Ë - Accent Selection");
    gtk_widget_set_margin_start(popup_label, 20);
    gtk_widget_set_margin_end(popup_label, 20);
    gtk_widget_set_margin_top(popup_label, 20);
    gtk_widget_set_margin_bottom(popup_label, 20);
    gtk_box_append(GTK_BOX(box), popup_label);

    GtkWidget *close_button = gtk_button_new_with_label("Close");
    g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(hide_popup), app);
    gtk_box_append(GTK_BOX(box), close_button);
}

void on_activate(GtkApplication *gtk_app, gpointer user_data) {
    AppData *app = (AppData *)user_data;

    app->window = gtk_application_window_new(gtk_app);
    gtk_window_set_title(GTK_WINDOW(app->window), "Accent Popup");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 400, 200);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(app->window), box);

    app->label = gtk_label_new("Hold the 'e' key for 2+ seconds to show accent popup");
    gtk_widget_set_margin_start(app->label, 20);
    gtk_widget_set_margin_end(app->label, 20);
    gtk_widget_set_margin_top(app->label, 20);
    gtk_widget_set_margin_bottom(app->label, 20);
    gtk_box_append(GTK_BOX(box), app->label);

    create_popup_window(app);
    gtk_window_present(GTK_WINDOW(app->window));

    setup_keyboard_monitoring(app);
}