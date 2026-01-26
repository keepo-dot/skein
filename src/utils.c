#include "types.h"
#include <gtk/gtk.h>

// global color constants.
const GdkRGBA COLOR_RED = {1.0, 0.0, 0.0, 1.0};
const GdkRGBA COLOR_ORANGE = {1.0, 0.65, 0.0, 1.0};
const GdkRGBA COLOR_YELLOW = {1.0, 1.0, 0.0, 1.0};
const GdkRGBA COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
const GdkRGBA COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
const GdkRGBA COLOR_INDIGO = {0.29, 0.0, 0.51, 1.0};
const GdkRGBA COLOR_VIOLET = {0.93, 0.51, 0.93, 1.0};
const GdkRGBA COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
const GdkRGBA COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};

static void draw_color_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                              gpointer button_color) {
  GdkRGBA *paint_color = (GdkRGBA *)button_color;
  gdk_cairo_set_source_rgba(cr, paint_color);
  cairo_paint(cr);
}

void draw_stitch_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                        gpointer user_data) {
  int stitch_type = GPOINTER_TO_INT(user_data);

  cairo_set_line_width(cr, 2.0);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

  double center_x = w / 2.0;
  double center_y = h / 2.0;
  double size = w * 0.6; // Scale relative to button size

  switch (stitch_type) {
  case STITCH_KNIT: // Vertical Line
    cairo_move_to(cr, center_x, center_y - (size / 2));
    cairo_line_to(cr, center_x, center_y + (size / 2));
    cairo_stroke(cr);
    break;

  case STITCH_PURL: // Horizontal Dash (or Dot)
    cairo_move_to(cr, center_x - (size / 2), center_y);
    cairo_line_to(cr, center_x + (size / 2), center_y);
    cairo_stroke(cr);
    break;

  case STITCH_YO: // Circle
    cairo_arc(cr, center_x, center_y, size / 3, 0, 2 * G_PI);
    cairo_stroke(cr);
    break;

  case STITCH_K2TOG: // Right Lean /
    cairo_move_to(cr, center_x + (size / 2),
                  center_y - (size / 2)); // Top Right
    cairo_line_to(cr, center_x - (size / 2), center_y + (size / 2)); // Bot Left
    cairo_stroke(cr);
    break;

  case STITCH_SSK: // Left Lean \ (Backslash)
    cairo_move_to(cr, center_x - (size / 2), center_y - (size / 2)); // Top Left
    cairo_line_to(cr, center_x + (size / 2),
                  center_y + (size / 2)); // Bot Right
    cairo_stroke(cr);
    break;
  }
}

static void draw_toolbar_icon(GtkDrawingArea *area, cairo_t *cr, int width,
                              int height, gpointer symbol_color) {
  GdkRGBA theme_color;
  gtk_widget_get_color(GTK_WIDGET(area), &theme_color);
  gdk_cairo_set_source_rgba(cr, &theme_color);
  draw_stitch_swatch(area, cr, width, height, symbol_color);
}

GtkWidget *create_button(ButtonInfo *info) {

  GtkWidget *button = gtk_toggle_button_new();

  // This creates tool buttons, ie with a NULL button_color.
  if (info->icon_name) {
    gtk_button_set_icon_name(GTK_BUTTON(button), info->icon_name);
    // This handles the color buttons. ie with a NULL icon_name.
  } else if (info->button_color != NULL) {
    GtkWidget *button_draw_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(button_draw_area),
                                   draw_color_swatch,
                                   (gpointer)info->button_color, NULL);
    gtk_button_set_child(GTK_BUTTON(button), button_draw_area);
    gtk_widget_set_size_request(button_draw_area, 16, 16);
    // this creates stitch buttons. ie with a stitch type > 0 (Empty Stitch).
  } else if (info->stitch_type != 0) {
    GtkWidget *button_draw_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(button_draw_area),
                                   draw_toolbar_icon,
                                   (gpointer)info->stitch_type, NULL);
    gtk_button_set_child(GTK_BUTTON(button), button_draw_area);
    gtk_widget_set_size_request(button_draw_area, 16, 16);

  } else if (info->label != NULL) {
    gtk_button_set_label(GTK_BUTTON(button), info->label);
  }
  gtk_widget_set_tooltip_text(
      button, info->label); // Set toolbar tip regardless of button type.

  return button;
}
