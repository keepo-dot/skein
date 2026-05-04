/*
 * Skein - A Knitting Pattern Editor
 * Copyright (C) 2026 keepo_dot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "canvas.h"
#include "skein_window.h"
#include "toolbar.h"
#include "types.h"
#include <gtk/gtk.h>
#include <stddef.h>
#include <stdlib.h>

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *main_window;
  GtkWidget *pattern_view;
  PatternData *grid;
  GtkWidget *container;
  GtkWidget *toolbar;
  AppState *app_state;

  // give activate the master state.
  app_state = (AppState *)user_data;
  grid = app_state->pattern;
  // create the toolbar.
  toolbar = create_toolbar(app_state);
  // create pattern view.
  pattern_view = create_pattern_view(app_state);
  // create container.
  container = create_app_container(pattern_view, toolbar);

  // create main window.
  main_window = create_main_window(app, container, app_state);
  app_state->main_window = main_window;

  gtk_window_present(GTK_WINDOW(main_window));
}

int main(int argc, char *argv[]) {
  GtkApplication *app;
  int status;
  AppState *master_state;
  UiState *ui_state;

  // Setup grid.
  PatternData *grid = calloc(1, sizeof(PatternData));

  grid->width = 100;
  grid->height = 100;
  grid->stitch_size = 30;

  // Allocate memory for AppState.
  master_state = calloc(1, sizeof(AppState));
  if (master_state == NULL) {
    g_print("Error: master state memory allocation failed.");
    return 1;
  }

  // Allocate memory for the grid.
  grid->stitch_data = calloc(grid->width * grid->height, sizeof(StitchData));
  if (grid->stitch_data == NULL) {
    g_print("Error: stitch data memory allocation failed.");
    return 1;
  }

  // Allocate memory for the HistoryTable.
  size_t ht_cap = 10;
  grid->history_table = calloc(1, sizeof(HistoryTable));
  grid->history_table->group = calloc(ht_cap, sizeof(ActionGroup));
  grid->history_table->table_size = ht_cap;

  // Allocate memory for the RepeatTable.
  size_t rt_cap = 5;
  grid->repeat_table = calloc(rt_cap, sizeof(RepeatTable));
  grid->repeat_table->repeat_section = calloc(rt_cap, sizeof(RepeatSection));
  grid->repeat_table->table_size = rt_cap;

  // Allocate memory for the UiState.
  ui_state = calloc(1, sizeof(UiState));
  if (ui_state == NULL) {
    g_print("Error: ui state memory allocation failed.");
    return 1;
  }
  // Allocate memory for ToolbarState.
  ui_state->toolbar_state = calloc(1, sizeof(ToolbarState));
  if (ui_state->toolbar_state == NULL) {
    g_print("Error: toolbar state memory allocation failed.");
    return 1;
  }

  master_state->pattern = grid;
  master_state->ui = ui_state;
  master_state->ui->pattern_data = master_state->pattern;

  // Draw window.
  app = gtk_application_new("com.github.keepo-dot.skein",
                            G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), master_state);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  free(grid->stitch_data);
  free(ui_state->toolbar_state);
  free(ui_state);
  free(master_state);

  g_object_unref(app);

  return status;
}
