#include <gtk/gtk.h>
#ifndef BUTTON_H
#define BUTTON_H

typedef struct {
  const char *label;
  void (*action)(GtkWidget *, gpointer);
} ButtonInfo;
#endif
