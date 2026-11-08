#ifndef CANVAS_RENDER_H
#define CANVAS_RENDER_H

#include "ui_types.h"
#include <cairo.h>
#include <gtk/gtk.h>

// Viewport widget creation
GtkWidget *create_pattern_view(AppState *app_state);

// Cairo draw routines for grid and swatches
void draw_repeat_outlines(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                          gpointer user_data);
void draw_color_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                       gpointer button_color);
void draw_stitch_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                        gpointer user_data);

#endif // !CANVAS_RENDER_H
