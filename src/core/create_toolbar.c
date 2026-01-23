#include "button.h"
#include "create_button.h"
#include <gtk/gtk.h>

GtkWidget *create_toolbar(void) {
  // define buttons on toolbar.
  ButtonInfo toolbar_buttons[] = {{"Move", NULL}, {"Paint Stitch", NULL}};

  // calculate button count, allows modular button addition later.
  int count = sizeof(toolbar_buttons) / sizeof(toolbar_buttons[0]);

  GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  for (int i = 0; i < count; i++) {
    GtkWidget *button = create_button(toolbar_buttons[i].label);
    gtk_box_append(GTK_BOX(toolbar), button);
  }
  return toolbar;
}
