#include "create_app_container.h"
#include "gtk/gtkshortcut.h"
#include <gtk/gtk.h>
#include <stdalign.h>

GtkWidget *create_app_container(GtkWidget *pattern_view, GtkWidget *toolbar) {
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(container), pattern_view);
  gtk_box_append(GTK_BOX(container), toolbar);
  return container;
}
