#include "app_state.h"
#include "create_app_container.h"
#include "create_main_window.h"
#include "create_pattern_view.h"
#include "create_toolbar.h"
#include "draw_grid.h"
#include <gtk/gtk.h>

void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *main_window;
  GtkWidget *pattern_view;
  PatternData *grid;
  GtkWidget *container;
  GtkWidget *toolbar;
  AppState *app_state;

  // give activate the master state.
  app_state = (AppState *)user_data;
  grid = app_state->pattern;
  /* DEBUG
    g_print("1. Entered Activate\n");
    app_state = (AppState *)user_data;
    if (!app_state) {
      g_print("Error: app_state is NULL\n");
      return;
    }

    g_print("2. Checking UI pointer\n");
    if (!app_state->ui) {
      g_print("Error: app_state->ui is NULL\n");
      return;
    }

    g_print("3. Checking Toolbar pointer\n");
    if (!app_state->ui->toolbar_state) {
      g_print("Error: toolbar_state is NULL\n");
      return;
    }

    g_print("4. Calling create_toolbar\n");
    toolbar = create_toolbar(app_state->ui->toolbar_state);
    g_print("5. Toolbar created\n");
  */
  // create the toolbar.
  toolbar = create_toolbar(app_state->ui->toolbar_state);
  // create pattern view.
  pattern_view = create_pattern_view(grid, draw_grid);
  // create container.
  container = create_app_container(pattern_view, toolbar);

  // create main window.
  main_window = create_main_window(app, container);

  g_print("Pattern loaded: %d x %d\n", grid->width, grid->height);
  gtk_window_present(GTK_WINDOW(main_window));
}
