#include "pattern.h"
#include "pattern_types.h"
#include "resources.h"
#include "ui_types.h"
#include "utils.h"

gboolean undo_action(GtkWidget *widget, GVariant *args, gpointer app_state) {
  AppState *state = (AppState *)app_state;
  HistoryTable *history_table = state->pattern->history_table;
  RepeatTable *repeat_table = state->pattern->repeat_table;
  if (history_table->current_position <= 0) {
    return false;
  }
  switch (
      history_table->group[history_table->current_position - 1].action_type) {
  case ACTION_STITCH_EDIT:
    if (history_table->current_position > 0) {
      ActionGroup *agroup =
          &history_table->group[history_table->current_position - 1];
      for (int i = agroup->group_size - 1; i >= 0; i--) {
        StitchData *data =
            &state->pattern->stitch_data[agroup->action[i].cell_num];
        data->stitch_color = agroup->action[i].before_state.stitch_color;
        data->stitch_type = agroup->action[i].before_state.stitch_type;
      }
      history_table->current_position -= 1;
      state->ui->redraw = true;
      return true;
    }
  case ACTION_REPEAT_ADD:
    if (repeat_table->num_repeats <= 0) {
      return false;
    }
    repeat_table->num_repeats--;
    history_table->current_position--;
    state->ui->redraw = true;
    return true;
  default:
    return false;
  }

  return false;
}

gboolean redo_action(GtkWidget *widget, GVariant *args, gpointer app_state) {
  AppState *state = (AppState *)app_state;
  HistoryTable *history_table = state->pattern->history_table;
  RepeatTable *repeat_table = state->pattern->repeat_table;
  switch (history_table->group[history_table->current_position].action_type) {
  case ACTION_STITCH_EDIT:
    if (history_table->current_position < history_table->table_size) {
      ActionGroup *agroup =
          &history_table->group[history_table->current_position];
      for (int i = 0; i < agroup->group_size; i++) {
        StitchData *data =
            &state->pattern->stitch_data[agroup->action[i].cell_num];
        data->stitch_color = agroup->action[i].after_state.stitch_color;
        data->stitch_type = agroup->action[i].after_state.stitch_type;
      }
      history_table->current_position += 1;
      state->ui->redraw = true;
      return true;
    }
  case ACTION_REPEAT_ADD:
    if (repeat_table->num_repeats < 0) {
      return false;
    }
    repeat_table->num_repeats++;
    history_table->current_position++;
    state->ui->redraw = true;
    return true;
  default:
    break;
  }

  return false;
}

static void apply_tool_to_cell(AppState *app_state, int index) {
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  PatternData *grid_data = app_state->pattern;

  switch (toolbar_state->active_mode) {
  case MODE_PAINT: {
    GdkRGBA initial_color = toolbar_state->active_color;

    if (initial_color.alpha != 0) {
      grid_data->stitch_data[index].stitch_color =
          TO_PATTERN_COLOR(toolbar_state->active_color);

      if (toolbar_state->active_stitch != STITCH_EMPTY) {
        grid_data->stitch_data[index].stitch_type =
            app_state->ui->toolbar_state->active_stitch;
      } else if (grid_data->stitch_data[index].stitch_type == STITCH_EMPTY) {
        grid_data->stitch_data[index].stitch_type = STITCH_KNIT;
      }
    }

    app_state->ui->redraw = true;
    break;
  }
  case MODE_STITCH:
    grid_data->stitch_data[index].stitch_type = toolbar_state->active_stitch;
    if (toolbar_state->active_color.alpha != 0.0) {
      grid_data->stitch_data[index].stitch_color =
          TO_PATTERN_COLOR(toolbar_state->active_color);
    }
    app_state->ui->redraw = true;
    break;
  case MODE_ERASE:
    grid_data->stitch_data[index].stitch_type = STITCH_EMPTY;
    grid_data->stitch_data[index].stitch_color = TO_PATTERN_COLOR(COLOR_EMPTY);
    app_state->ui->redraw = true;
    break;
  case MODE_PICKER:
    if (grid_data->stitch_data[index].stitch_color.alpha > 0.0) {
      toolbar_state->active_color =
          TO_GDK_RGBA(grid_data->stitch_data[index].stitch_color);
    }
    break;
  case MODE_FILL:
    fill_cells(app_state);
    break;
  default:
    break;
  }
}

// handles release events.
void on_drag_end(GtkGestureDrag *gesture, double offset_x, double offset_y,
                 AppState *app_state) {
  // g_print("1. Entered on_drag_end\n");
  UiState *ui = app_state->ui;
  PatternData *grid_data = app_state->pattern;
  int column = (int)((ui->mouse_start_x + ui->camera_x) /
                     app_state->pattern->stitch_size);
  int row = (int)((ui->mouse_start_y + ui->camera_y) /
                  app_state->pattern->stitch_size);

  if ((column >= 0 && column < grid_data->width) &&
      (row >= 0 && row < grid_data->height)) {
    ToolbarState *toolbar_state = app_state->ui->toolbar_state;
    PatternData *pattern_data = app_state->pattern;
    HistoryTable *history = app_state->pattern->history_table;
    RepeatTable *repeat_table = app_state->pattern->repeat_table;
    ActionGroup *last_group = &history->group[history->current_position - 1];

    if (toolbar_state && toolbar_state->active_mode == MODE_REPEAT) {
      show_repeat_dialog(app_state->main_window, app_state);
      return;
    }
    bool should_discard = false;
    if (last_group->group_size == 0) {
      should_discard = true;
    } else if (last_group->group_size == 1) {
      StitchDelta *first_delta =
          &history->group[history->current_position - 1].action[0];
      GdkRGBA before_color =
          TO_GDK_RGBA(first_delta->before_state.stitch_color);
      GdkRGBA after_color = TO_GDK_RGBA(first_delta->after_state.stitch_color);
      if (last_group->group_size == 0 ||
          (last_group->group_size == 1 && before_color.red == after_color.red &&
           before_color.green == after_color.green &&
           before_color.blue == after_color.blue &&
           before_color.alpha == after_color.alpha &&
           first_delta->before_state.stitch_type ==
               first_delta->after_state.stitch_type)) {
        should_discard = true;
      }
      if (should_discard) {
        if (last_group->action != NULL) {
          free(last_group->action);
          last_group->action = NULL;
        }
        history->current_position -= 1;
        history->history_count -= 1;
      }
    }
  }
}

// handles drag events like painting strokes and moving.
void on_drag_update(GtkGestureDrag *gesture, double offset_x, double offset_y,
                    AppState *app_state) {
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  UiState *ui = app_state->ui;
  PatternData *grid_data = app_state->pattern;
  GtkWidget *area =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

  double current_mouse_x = ui->mouse_start_x + offset_x;
  double current_mouse_y = ui->mouse_start_y + offset_y;
  int column = (int)((current_mouse_x + ui->camera_x) / grid_data->stitch_size);
  int row = (int)((current_mouse_y + ui->camera_y) / grid_data->stitch_size);

  if (toolbar_state && toolbar_state->active_mode == MODE_REPEAT) {
    ui->temp_repeat_end = row;
    ui->redraw = true;
    return;
  }

  if (toolbar_state && toolbar_state->active_mode == MODE_MOVE) {
    double new_camera_pos_x = ui->drag_start_x - offset_x;
    double new_camera_pos_y = ui->drag_start_y - offset_y;
    int width = gtk_widget_get_width(area);
    int height = gtk_widget_get_height(area);
    int max_x = (grid_data->width * grid_data->stitch_size) - width;
    int max_y = (grid_data->height * grid_data->stitch_size) - height;

    ui->camera_x = MAX(0, MIN(new_camera_pos_x, MAX(0, max_x)));
    ui->camera_y = MAX(0, MIN(new_camera_pos_y, MAX(0, max_y)));
    ui->redraw = true;
    return;
  }

  if (column >= 0 && column < grid_data->width && row >= 0 &&
      row < grid_data->height) {
    int index = (row * grid_data->width) + column;

    PatternColor color = {0};
    StitchType stitch = 0;

    if (toolbar_state) {
      color = TO_PATTERN_COLOR(toolbar_state->active_color);
      stitch = toolbar_state->active_stitch;
      if (stitch == STITCH_EMPTY) {
        stitch = STITCH_KNIT;
      }
    }

    pattern_apply_stitch_drag(grid_data, index, color, stitch);
    ui->redraw = true;
  }
}
// handles single click events like drawing a color,or drawing a stitch.
void on_drag_begin(GtkGestureDrag *gesture, double start_x, double start_y,
                   AppState *app_state) {
  UiState *ui = app_state->ui;
  PatternData *grid_data = app_state->pattern;
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;

  int active_tool = 0;
  PatternColor color = {0};
  StitchType stitch = 0;

  ui->mouse_start_x = start_x;
  ui->mouse_start_y = start_y;

  if (toolbar_state && toolbar_state->active_mode == MODE_MOVE) {
    ui->drag_start_x = ui->camera_x;
    ui->drag_start_y = ui->camera_y;
    return;
  }

  if (toolbar_state && toolbar_state->active_mode == MODE_REPEAT) {
    int row = (int)((start_y + ui->camera_y) / grid_data->stitch_size);
    toolbar_state->is_drawing_repeat = true;
    ui->temp_repeat_start = row;
    ui->temp_repeat_end = row;
    return;
  }

  int column = (int)((start_x + ui->camera_x) / grid_data->stitch_size);
  int row = (int)((start_y + ui->camera_y) / grid_data->stitch_size);

  if (column >= 0 && column < grid_data->width && row >= 0 &&
      row < grid_data->height) {
    int index = (row * grid_data->width) + column;

    if (toolbar_state) {
      active_tool = toolbar_state->active_mode;
      color = TO_PATTERN_COLOR(toolbar_state->active_color);
      stitch = toolbar_state->active_stitch;
      if (toolbar_state->active_stitch == STITCH_EMPTY) {
        stitch = STITCH_KNIT;
      }
    }
    pattern_apply_stitch(grid_data, index, color, stitch);
  }
  ui->redraw = true;
}

gboolean on_scroll(GtkEventControllerScroll *controller, double dx, double dy,
                   AppState *app_state) {
  UiState *ui = app_state->ui;
  GdkModifierType state = gtk_event_controller_get_current_event_state(
      GTK_EVENT_CONTROLLER(controller));
  GtkWidget *area =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(controller));
  int width = gtk_widget_get_width(area);
  int height = gtk_widget_get_height(area);
  int old_size = app_state->pattern->stitch_size;
  double center_x = (width / 2.0) + ui->camera_x;
  double center_y = (height / 2.0) + ui->camera_y;
  double grid_x = center_x / old_size;
  double grid_y = center_y / old_size;
  if (!(state & GDK_CONTROL_MASK)) {
    return FALSE;
  }
  app_state->pattern->stitch_size -= (dy * 5);
  if (app_state->pattern->stitch_size < 5) {
    app_state->pattern->stitch_size = 5;
  } else if (app_state->pattern->stitch_size > 100) {
    app_state->pattern->stitch_size = 100;
  }

  ui->camera_x = (grid_x * app_state->pattern->stitch_size) - (width / 2.0);
  ui->camera_y = (grid_y * app_state->pattern->stitch_size) - (height / 2.0);
  int max_x =
      (app_state->pattern->width * app_state->pattern->stitch_size) - width;
  int max_y =
      (app_state->pattern->height * app_state->pattern->stitch_size) - height;

  ui->camera_x = MAX(0, MIN(ui->camera_x, MAX(0, max_x)));
  ui->camera_y = MAX(0, MIN(ui->camera_y, MAX(0, max_y)));
  ui->redraw = true;
  return TRUE;
}

gboolean on_motion(GtkEventControllerMotion *controller, double mx, double my,
                   AppState *app_state) {
  UiState *ui = app_state->ui;
  PatternData *grid_data = app_state->pattern;
  RepeatTable *repeat_table = app_state->pattern->repeat_table;
  int hover_row = (int)((my + ui->camera_y) / grid_data->stitch_size);
  int current_hit = -1;
  for (int i = 0; i < app_state->pattern->repeat_table->num_repeats; i++) {
    int s_row = repeat_table->repeat_section[i].start_row;
    int e_row = repeat_table->repeat_section[i].end_row;
    int top_row = (s_row < e_row) ? s_row : e_row;
    int bottom_row = (s_row < e_row) ? e_row : s_row;
    if (hover_row <= bottom_row && hover_row >= top_row) {
      current_hit = i;
      break;
    }
  }
  if (current_hit != ui->hovered_repeat_index) {
    ui->hovered_repeat_index = current_hit;
    ui->redraw = true;
  }
  return TRUE;
}
