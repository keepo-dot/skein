#include <gtk/gtk.h>

GtkWidget *create_app_container(GtkWidget *pattern_view, GtkWidget *toolbar) {
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(container), pattern_view);
  gtk_box_append(GTK_BOX(container), toolbar);
  return container;
}

GtkWidget *create_main_window(GtkApplication *app, GtkWidget *container) {
  GtkWidget *window;
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Skein - Knitting Pattern Maker");
  gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);
  gtk_window_set_child(GTK_WINDOW(window), container);
  return window;
}
