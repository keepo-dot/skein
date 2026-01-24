#include <gtk/gtk.h>

GtkWidget *create_main_window(GtkApplication *app, GtkWidget *container) {
  GtkWidget *window;
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "KnitPatC");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
  gtk_window_set_child(GTK_WINDOW(window), container);
  return window;
}
