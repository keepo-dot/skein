#include "dialogs.h"
#include "pattern.h"
#include "ui_types.h"
#include <gtk/gtk.h>
#include <stdlib.h>

void on_repeat_confirm(GtkWidget *button, AppState *app_state) {
  RepeatTable *repeat_table = app_state->pattern->repeat_table;
  HistoryTable *history = app_state->pattern->history_table;

  GtkTextView *text_view = g_object_get_data(G_OBJECT(button), "text");
  GtkWindow *dialog_window =
      g_object_get_data(G_OBJECT(button), "dialog-window");
  GtkTextBuffer *buf = gtk_text_view_get_buffer(text_view);
  GtkTextIter buf_start;
  GtkTextIter buf_end;
  gtk_text_buffer_get_bounds(buf, &buf_start, &buf_end);
  char *text = gtk_text_buffer_get_text(buf, &buf_start, &buf_end, FALSE);

  if (repeat_table->num_repeats >= repeat_table->table_size) {
    size_t new_cap = (repeat_table->table_size * 2);
    if (new_cap == 0) {
      new_cap = 1;
    }
    repeat_table->repeat_section =
        realloc(repeat_table->repeat_section, new_cap * sizeof(RepeatSection));
    for (size_t i = repeat_table->table_size; i < new_cap; i++) {
      repeat_table->repeat_section[i].start_row = 0;
      repeat_table->repeat_section[i].end_row = 0;
    }
    repeat_table->table_size = new_cap;
  }
  repeat_table->repeat_section[repeat_table->num_repeats].start_row =
      app_state->ui->temp_repeat_start;
  repeat_table->repeat_section[repeat_table->num_repeats].end_row =
      app_state->ui->temp_repeat_end;
  repeat_table->repeat_section[repeat_table->num_repeats].repeat_info = text;
  repeat_table->num_repeats++;

  history->group[history->current_position - 1].action_type = ACTION_REPEAT_ADD;
  history->group[history->current_position - 1].group_size = 0;
  history->group[history->current_position - 1].action = NULL;
  app_state->ui->redraw = true;
  gtk_window_destroy(dialog_window);
}

void show_repeat_dialog(GtkWidget *main_window, AppState *app_state) {
  (void)main_window;

  GtkWidget *repeat_dialog = gtk_window_new();
  gtk_window_set_modal(GTK_WINDOW(repeat_dialog), true);
  gtk_window_set_transient_for(GTK_WINDOW(repeat_dialog),
                               GTK_WINDOW(app_state->main_window));
  gtk_window_set_default_size(GTK_WINDOW(repeat_dialog), 400, 200);
  GtkWidget *input_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(repeat_dialog), input_container);
  GtkWidget *text_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_vexpand(text_container, true);
  GtkWidget *input_label = gtk_label_new("Repeat Info:");
  GtkWidget *view = gtk_text_view_new();
  GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
  gtk_text_buffer_set_text(buf, "Insert repeat info here. (remove this text)",
                           -1);
  gtk_widget_set_hexpand(view, true);
  GtkWidget *scrolled = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), view);

  gtk_box_append(GTK_BOX(text_container), input_label);
  gtk_box_append(GTK_BOX(text_container), scrolled);
  GtkWidget *button_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *confirm_button = gtk_button_new_with_label("Confirm");
  GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
  gtk_box_append(GTK_BOX(button_container), cancel_button);
  gtk_box_append(GTK_BOX(button_container), confirm_button);
  gtk_box_append(GTK_BOX(input_container), text_container);
  gtk_box_append(GTK_BOX(input_container), button_container);
  g_signal_connect_swapped(cancel_button, "clicked",
                           G_CALLBACK(gtk_window_destroy), repeat_dialog);
  g_object_set_data(G_OBJECT(confirm_button), "text", view);
  g_object_set_data(G_OBJECT(confirm_button), "dialog-window", repeat_dialog);
  g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_repeat_confirm),
                   app_state);
  gtk_window_present(GTK_WINDOW(repeat_dialog));
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
  state->ui->redraw = true;
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

gboolean on_repeat_hover(GtkWidget *draw_area, gint mouse_x, gint mouse_y,
                         gboolean keyboard_trigger, GtkTooltip *tooltip,
                         AppState *app_state) {
  (void)draw_area;
  (void)mouse_x;
  (void)keyboard_trigger;
  RepeatTable *repeat_table = app_state->pattern->repeat_table;
  int row_idx =
      ((mouse_y - app_state->ui->offset_y) / app_state->pattern->stitch_size);
  for (size_t i = 0; i < repeat_table->num_repeats; i++) {
    if (row_idx >= repeat_table->repeat_section[i].start_row &&
        row_idx <= repeat_table->repeat_section[i].end_row) {
      gtk_tooltip_set_text(tooltip,
                           repeat_table->repeat_section[i].repeat_info);
      return TRUE;
    }
  }
  return FALSE;
}
