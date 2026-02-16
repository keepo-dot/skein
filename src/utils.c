#include "resources.h"
#include "types.h"
#include <gtk/gtk.h>
#include <json-glib-1.0/json-glib/json-glib.h>

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
void pattern_json_load(char *filename, PatternData *current_canvas) {
  GError *error = NULL;
  JsonParser *parser = json_parser_new();
  json_parser_load_from_file(parser, filename, &error);
  if (error) {
    g_print("Unable to load file '%s': %s\n", filename, error->message);
    g_error_free(error);
    return;
  }
  JsonNode *root = json_parser_get_root(parser);
  JsonReader *reader = json_reader_new(root);
  json_reader_read_member(reader, "pattern_width");
  int new_width = json_reader_get_int_value(reader);
  json_reader_end_member(reader);
  json_reader_read_member(reader, "pattern_height");
  int new_height = json_reader_get_int_value(reader);
  json_reader_end_member(reader);

  if (new_width != current_canvas->width ||
      new_height != current_canvas->height) {
    free(current_canvas->stitch_data);
    current_canvas->stitch_data =
        calloc((size_t)(new_width * new_height), sizeof(StitchData));
    current_canvas->width = new_width;
    current_canvas->height = new_height;
  }
  json_reader_read_member(reader, "stitch_data");
  int index = 0;
  int num_cells = (new_width * new_height);
  while (index < num_cells) {
    json_reader_read_element(reader, index);
    json_reader_read_member(reader, "stitch_type");
    current_canvas->stitch_data[index].stitch_type =
        json_reader_get_int_value(reader);
    json_reader_end_member(reader);

    json_reader_read_member(reader, "stitch_color");
    json_reader_read_element(reader, 0);
    current_canvas->stitch_data[index].stitch_color.red =
        json_reader_get_double_value(reader);
    json_reader_end_element(reader);
    json_reader_read_element(reader, 1);
    current_canvas->stitch_data[index].stitch_color.green =
        json_reader_get_double_value(reader);
    json_reader_end_element(reader);
    json_reader_read_element(reader, 2);
    current_canvas->stitch_data[index].stitch_color.blue =
        json_reader_get_double_value(reader);
    json_reader_end_element(reader);
    json_reader_read_element(reader, 3);
    current_canvas->stitch_data[index].stitch_color.alpha =
        json_reader_get_double_value(reader);
    json_reader_end_element(reader);
    json_reader_end_member(reader);
    json_reader_end_element(reader);
    index++;
  }
  json_reader_end_member(reader);
  current_canvas->redraw = true;
  g_object_unref(reader);
  g_object_unref(parser);
  return;
}

JsonBuilder *pattern_json_builder(PatternData *pattern) {
  JsonBuilder *builder = json_builder_new();

  json_builder_begin_object(builder);
  json_builder_set_member_name(builder, "author");
  json_builder_add_string_value(builder, "keepo-dot");
  json_builder_set_member_name(builder, "revision");
  json_builder_add_int_value(builder, 1);
  json_builder_set_member_name(builder, "pattern_width");
  json_builder_add_int_value(builder, pattern->width);
  json_builder_set_member_name(builder, "pattern_height");
  json_builder_add_int_value(builder, pattern->height);
  json_builder_set_member_name(builder, "stitch_data");
  json_builder_begin_array(builder);

  for (size_t i = 0; i < (pattern->width * pattern->height); i++) {
    StitchData *current_stitch = &pattern->stitch_data[i];
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "stitch_type");
    json_builder_add_int_value(builder, current_stitch->stitch_type);
    json_builder_set_member_name(builder, "stitch_color");
    json_builder_begin_array(builder);
    json_builder_add_double_value(builder, current_stitch->stitch_color.red);
    json_builder_add_double_value(builder, current_stitch->stitch_color.green);
    json_builder_add_double_value(builder, current_stitch->stitch_color.blue);
    json_builder_add_double_value(builder, current_stitch->stitch_color.alpha);
    json_builder_end_array(builder);
    json_builder_end_object(builder);
  }
  json_builder_end_array(builder);
  json_builder_end_object(builder);

  return builder;
}

void pattern_json_save(JsonBuilder *builder, char *file_path) {
  JsonNode *root = json_builder_get_root(builder);
  JsonGenerator *generator = json_generator_new();
  json_generator_set_root(generator, root);
  json_generator_set_pretty(generator, true);
  json_generator_to_file(generator, file_path, false);
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

void on_save_dialog_finish(GObject *file_dialog, GAsyncResult *save_file,
                           gpointer app_state_p) {
  AppState *app_state = (AppState *)app_state_p;
  GtkWidget *main_window = app_state->main_window;
  PatternData *pattern = app_state->pattern;
  GFile *file_to_save = gtk_file_dialog_save_finish(
      GTK_FILE_DIALOG(file_dialog), save_file, NULL);
  if (file_to_save == NULL) {
    return;
  }
  char *path = g_file_get_path(file_to_save);
  if (g_str_has_suffix(path, ".skn")) {
    pattern_json_save(pattern_json_builder(pattern), path);
    g_free(path);
  } else {
    char *fixed_path = g_strdup_printf("%s.skn", path);
    pattern_json_save(pattern_json_builder(pattern), fixed_path);
    g_free(fixed_path);
  }
  GtkAlertDialog *alert = gtk_alert_dialog_new("Pattern saved successfully!");
  gtk_alert_dialog_show(alert, GTK_WINDOW(main_window));
  g_object_unref(alert);

  g_object_unref(file_to_save);
}
void on_load_dialog_finish(GObject *file_dialog, GAsyncResult *file_to_open,
                           gpointer app_state_p) {
  AppState *app_state = (AppState *)app_state_p;
  GtkWidget *main_window = app_state->main_window;
  GError *error = NULL;
  GFile *file_to_load = gtk_file_dialog_open_finish(
      GTK_FILE_DIALOG(file_dialog), file_to_open, NULL);
  if (file_to_load == NULL) {
    if (error && error->code != G_IO_ERROR_CANCELLED) {
      GtkAlertDialog *null_alert =
          gtk_alert_dialog_new("File load failed: %s", error->message);
      gtk_alert_dialog_show(null_alert, GTK_WINDOW(main_window));
      g_object_unref(null_alert);
    }
    g_clear_error(&error);
    return;
  }
  char *path = g_file_get_path(file_to_load);
  if (path == NULL) {
    GtkAlertDialog *null_path =
        gtk_alert_dialog_new("File load failed: filepath is null or corrupt.");
    gtk_alert_dialog_show(null_path, GTK_WINDOW(main_window));
    g_object_unref(null_path);
    return;
  }
  pattern_json_load(path, app_state->pattern);
  GtkAlertDialog *alert = gtk_alert_dialog_new("Pattern loaded successfully!");
  gtk_alert_dialog_show(alert, GTK_WINDOW(main_window));
  g_object_unref(alert);
  g_free(path);
  g_object_unref(file_to_load);
}

void on_empty_save_response(GObject *source_object, GAsyncResult *res,
                            gpointer user_data) {
  AppState *app_state = (AppState *)user_data;
  GtkAlertDialog *alert = GTK_ALERT_DIALOG(source_object);
  GError *error = NULL;

  int response = gtk_alert_dialog_choose_finish(alert, res, &error);

  if (error) {
    g_print("Error: %s\n", error->message);
    g_error_free(error);
    return;
  }

  if (response == 1) {
    GtkWidget *main_window = app_state->main_window;
    GtkFileDialog *save_dialog = setup_file_dialog("Save Pattern");
    gtk_file_dialog_save(save_dialog, GTK_WINDOW(main_window), NULL,
                         on_save_dialog_finish, app_state);
  }
}
