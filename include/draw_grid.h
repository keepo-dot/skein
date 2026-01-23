#include <gtk/gtk.h>
#ifndef DRAW_GRID_H
#define DRAW_GRID_H

void draw_grid(GtkDrawingArea *area, cairo_t *cr, int width, int height,
               gpointer user_data);
#endif
