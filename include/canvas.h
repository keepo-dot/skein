#ifndef SKEIN_CANVAS_H
#define SKEIN_CANVAS_H
#include "types.h"
#include <gtk/gtk.h>
GtkWidget *create_pattern_view(AppState *app_state);

gboolean undo_action(GtkWidget *widget, GVariant *args, gpointer app_state);
gboolean redo_action(GtkWidget *widget, GVariant *args, gpointer app_state);
#endif
