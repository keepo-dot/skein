#include "resources.h"
#include "types.h"
#include <gtk/gtk.h>
#include <json-glib-1.0/json-glib/json-glib.h>

void draw_color_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
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
  case STITCH_KNIT: // No line, knit is default.
    break;

  case STITCH_PURL: // Horizontal Dash (or Dot)
    cairo_move_to(cr, center_x - (size / 2), center_y);
    cairo_line_to(cr, center_x + (size / 2), center_y);
    cairo_stroke(cr);
    break;

  case STITCH_M1: // New: A twisted loop "e"
    // Top loop
    cairo_arc(cr, center_x, center_y - (size / 4), size / 4, 0, 2 * G_PI);
    // Legs crossing at bottom
    cairo_move_to(cr, center_x - (size / 4), center_y + (size / 2));
    cairo_line_to(cr, center_x + (size / 4), center_y); // cross up
    cairo_stroke(cr);
    break;

  case STITCH_SLIP: // New: A "V" shape
    cairo_move_to(cr, center_x - (size / 3), center_y - (size / 3));
    cairo_line_to(cr, center_x, center_y + (size / 3)); // Down to point
    cairo_line_to(cr, center_x + (size / 3),
                  center_y - (size / 3)); // Up to right
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
  GtkWidget *button = NULL;
  // check button type as not to create the wrong one.
  if (info->is_toggle == true) {
    button = gtk_toggle_button_new();
  } else {
    button = gtk_button_new();
  }
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

GtkFileDialog *setup_file_dialog(char *window_title) {
  GListStore *filter_list = g_list_store_new(GTK_TYPE_FILE_FILTER);
  GtkFileDialog *new_dialog = gtk_file_dialog_new();
  gtk_file_dialog_set_title(new_dialog, window_title);
  GtkFileFilter *file_filter = gtk_file_filter_new();
  gtk_file_filter_set_name(file_filter, FILTER_NAME);
  gtk_file_filter_add_pattern(file_filter, FILE_TYPE);
  g_list_store_append(filter_list, file_filter);
  gtk_file_dialog_set_default_filter(new_dialog, file_filter);
  g_object_unref(filter_list);
  return new_dialog;
}

void pattern_reset_size(PatternData *pattern, int new_width, int new_height) {
  free(pattern->stitch_data);
  pattern->stitch_data =
      calloc((size_t)(new_width * new_height), sizeof(StitchData));
  pattern->width = new_width;
  pattern->height = new_height;
}

void on_new_pattern_confirm(GtkWidget *button, gpointer app_state) {
  AppState *state = (AppState *)app_state;
  GtkWidget *w_spin = g_object_get_data(G_OBJECT(button), "w-spin");
  GtkWidget *h_spin = g_object_get_data(G_OBJECT(button), "h-spin");
  GtkWidget *dialog_window =
      g_object_get_data(G_OBJECT(button), "dialog-window");
  int new_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w_spin));
  int new_height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(h_spin));
  pattern_reset_size(state->pattern, new_width, new_height);
  state->pattern->redraw = true;
  gtk_window_destroy(GTK_WINDOW(dialog_window));
}

void show_new_pattern_dialog(GtkWidget *main_window, AppState *app_state) {
  GtkWidget *new_pattern_dialog = gtk_window_new();
  gtk_window_set_modal(GTK_WINDOW(new_pattern_dialog), true);
  gtk_window_set_transient_for(GTK_WINDOW(new_pattern_dialog),
                               GTK_WINDOW(main_window));
  GtkWidget *input_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(new_pattern_dialog), input_container);

  GtkWidget *width_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *width_label = gtk_label_new("Width:");
  GtkWidget *width_spinner = gtk_spin_button_new_with_range(1, 1000, 1);
  gtk_box_append(GTK_BOX(width_container), width_label);
  gtk_box_append(GTK_BOX(width_container), width_spinner);

  GtkWidget *height_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *height_label = gtk_label_new("Height:");
  GtkWidget *height_spinner = gtk_spin_button_new_with_range(1, 1000, 1);
  gtk_box_append(GTK_BOX(height_container), height_label);
  gtk_box_append(GTK_BOX(height_container), height_spinner);

  GtkWidget *button_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
  GtkWidget *create_button = gtk_button_new_with_label("Create");
  gtk_box_append(GTK_BOX(button_container), cancel_button);
  gtk_box_append(GTK_BOX(button_container), create_button);

  gtk_box_append(GTK_BOX(input_container), width_container);
  gtk_box_append(GTK_BOX(input_container), height_container);
  gtk_box_append(GTK_BOX(input_container), button_container);
  g_signal_connect_swapped(cancel_button, "clicked",
                           G_CALLBACK(gtk_window_destroy), new_pattern_dialog);
  g_object_set_data(G_OBJECT(create_button), "w-spin", width_spinner);
  g_object_set_data(G_OBJECT(create_button), "h-spin", height_spinner);
  g_object_set_data(G_OBJECT(create_button), "dialog-window",
                    new_pattern_dialog);
  g_signal_connect(create_button, "clicked", G_CALLBACK(on_new_pattern_confirm),
                   app_state);
  gtk_window_present(GTK_WINDOW(new_pattern_dialog));
}

bool is_pattern_empty(PatternData *pattern) {
  int total_cells = pattern->width * pattern->height;

  for (int i = 0; i < total_cells; i++) {
    if (pattern->stitch_data[i].stitch_type != STITCH_EMPTY ||
        pattern->stitch_data[i].stitch_color.alpha > 0.0) {
      return false;
    }
  }
  return true;
}
