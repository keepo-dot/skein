#include "button.h"
#include <gtk/gtk.h>

GtkWidget *create_button(const char *label) {
  return gtk_button_new_with_label(label);
}
