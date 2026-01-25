#include <gtk/gtk.h>

#ifndef DRAW_COLOR_SWATCH
#define DRAW_COLOR_SWATCH

void draw_color_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                       gpointer button_color);

#endif // !DRAW_COLOR_SWATCH
