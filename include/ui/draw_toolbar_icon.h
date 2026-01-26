#include "glib.h"
#include <gtk/gtk.h>

#ifndef DRAW_TOOLBAR_ICON

void draw_toolbar_icon(GtkDrawingArea *area, cairo_t *cr, int width, int height,
                       gpointer symbol_color);

#endif // !DRAW_TOOLBAR_ICON
