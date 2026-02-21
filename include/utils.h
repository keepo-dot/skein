#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include <gtk/gtk.h>
#include <json-glib-1.0/json-glib/json-glib.h>

void draw_stitch_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                        gpointer user_data);
GtkWidget *create_button(ButtonInfo *info);
bool is_pattern_empty(PatternData *pattern);
void pattern_reset_size(PatternData *pattern, int new_width, int new_height);
void show_new_pattern_dialog(GtkWidget *main_window, AppState *app_state);
void on_new_pattern_confirm(GtkWidget *button, gpointer app_state);
GtkFileDialog *setup_file_dialog(char *window_title);
#endif // !UTILS_H
