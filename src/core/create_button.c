#include "button.h"
#include <gtk/gtk.h>
#include <stdalign.h>

GtkWidget *create_button(ButtonInfo *info) {

  GtkWidget *button = gtk_toggle_button_new();
  gtk_button_set_icon_name(GTK_BUTTON(button), info->icon_name);
  gtk_widget_set_tooltip_text(button, info->label);

  return button;
}
