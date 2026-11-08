#include "pattern_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool history_cleanup_duplicates(HistoryTable *ht) {
  if (!ht || !ht->group) {
    return false;
  }
  if (ht->current_position == 0) {
    return false;
  }

  ActionGroup *last_group = &ht->group[ht->current_position - 1];

  if (last_group->action != NULL) {
    free(last_group->action);
    last_group->action = NULL;
    last_group->action_type = 0;
    last_group->group_size = 0;
  }
  ht->current_position -= 1;
  ht->history_count -= 1;
  return true;
}

static void history_truncate_future(HistoryTable *ht) {
  if (!ht || !ht->group) {
    return;
  }
  if (ht->current_position < ht->history_count) {
    for (size_t i = ht->current_position; i < ht->history_count; i++) {
      free(ht->group[i].action);
      ht->group[i].action = NULL;
      ht->group[i].group_size = 0;
      ht->group[i].action_type = 0;
    }
    ht->history_count = ht->current_position;
  }
  return;
}

static bool history_ensure_capacity(HistoryTable *ht) {
  if (!ht) {
    return false;
  }
  if (ht->table_size == ht->current_position) {
    size_t new_cap = ht->table_size * 2;
    if (!new_cap) {
      new_cap = 1;
    }
    ActionGroup *new_group = realloc(ht->group, new_cap * sizeof(ActionGroup));
    if (new_group == NULL) {
      // TODO: add error handling a la Go. I like error handling. Until then
      // just print to console.
      fprintf(stderr, "Error: Failed to allocate memory for history group.\n");
      return false;
    }
    ht->group = new_group;

    for (size_t i = ht->table_size; i < new_cap; i++) {
      ht->group[i].group_size = 0;
      ht->group[i].action = NULL;
      ht->group[i].action_type = 0;
    }
    ht->table_size = new_cap;
  }
  return true;
}

void pattern_apply_stitch(PatternData *pattern, int index, PatternColor color,
                          StitchType stitch) {
  // Guard checks.
  if (!pattern || !pattern->history_table || !pattern->stitch_data) {
    return;
  }

  HistoryTable *history_table = pattern->history_table;
  // If the current history table position is less than the total count, wipe
  // the history ahead so new actions replace the old future.
  if (history_table->current_position < history_table->history_count) {
    history_truncate_future(history_table);
  }
  // Resize the history table if it's full.
  if (!history_ensure_capacity(history_table)) {
    return;
  }
  // Apply change to given cell.
  ActionGroup *agroup = &history_table->group[history_table->current_position];
  agroup->action = calloc(1, sizeof(StitchDelta));
  if (agroup->action == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for action group.\n");
    return;
  }
  agroup->group_size = 1;
  agroup->action_type = ACTION_STITCH_EDIT;
  agroup->action->cell_num = index;
  agroup->action->before_state = pattern->stitch_data[index];
  pattern->stitch_data[index].stitch_type = stitch;
  pattern->stitch_data[index].stitch_color = color;
  agroup->action->after_state = pattern->stitch_data[index];
  history_table->history_count++;
  history_table->current_position++;
}

void pattern_apply_stitch_drag(PatternData *pattern, int index,
                               PatternColor color, StitchType stitch) {
  // Guard checks.
  if (!pattern || !pattern->history_table || !pattern->stitch_data) {
    return;
  }
  HistoryTable *history_table = pattern->history_table;

  if (!history_table || !history_table->group ||
      history_table->current_position == 0) {
    return;
  }
  ActionGroup *agroup =
      &history_table->group[history_table->current_position - 1];
  // Ignore duplicate cells with no changes.
  if (agroup->action && agroup->group_size > 0 &&
      agroup->action[agroup->group_size - 1].cell_num == index) {
    return;
  }

  StitchDelta *new_delta =
      realloc(agroup->action, (agroup->group_size + 1) * sizeof(StitchDelta));
  if (new_delta == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for SitchDelta.\n");
    return;
  }
  agroup->group_size++;
  agroup->action_type = ACTION_STITCH_EDIT;
  agroup->action = new_delta;
  agroup->action[agroup->group_size - 1].cell_num = index;
  agroup->action[agroup->group_size - 1].before_state =
      pattern->stitch_data[index];
  pattern->stitch_data[index].stitch_color = color;
  pattern->stitch_data[index].stitch_type = stitch;
  agroup->action[agroup->group_size - 1].after_state =
      pattern->stitch_data[index];
}

void pattern_apply_stitch_end(PatternData *pattern) {
  if (!pattern || !pattern->history_table || !pattern->repeat_table) {
    return;
  }

  HistoryTable *history_table = pattern->history_table;
  if (!history_table->group) {
    return;
  }
  if (history_table->current_position == 0) {
    return;
  }
  ActionGroup *last_group =
      &history_table->group[history_table->current_position - 1];
  if (last_group->group_size == 0) {
    history_cleanup_duplicates(history_table);
  } else if (last_group->group_size == 1) {
    StitchDelta *first_delta =
        &history_table->group[history_table->current_position - 1].action[0];
    PatternColor b_color = first_delta->before_state.stitch_color;
    PatternColor a_color = first_delta->after_state.stitch_color;
    StitchType b_stitch = first_delta->before_state.stitch_type;
    StitchType a_stitch = first_delta->after_state.stitch_type;
    bool same_color = !memcmp(&b_color, &a_color, sizeof(PatternColor));
    bool same_stitch = !memcmp(&b_stitch, &a_stitch, sizeof(StitchType));
    if (same_color && same_stitch) {
      if (!history_cleanup_duplicates(history_table)) {
        fprintf(stderr, "History duplicate cleanup failed.\n");
      }
    }
  }
}

void pattern_apply_fill(PatternData *pattern, int col, int row,
                        PatternColor color, StitchType stitch) {
  if (!pattern || !pattern->history_table || !pattern->history_table->group) {
    return;
  }
  int index = (row * pattern->width) + col;
  // click bounds check, otherwise off grid clicks cause segfaults.
  if (col < 0 || col >= pattern->width || row < 0 || row >= pattern->height) {
    return;
  }
  HistoryTable *ht = pattern->history_table;
  // if clicked cell is the same as what's being filled, return to avoid
  // infinite loop.

  PatternColor cell_color = pattern->stitch_data[index].stitch_color;
  StitchType cell_stitch = pattern->stitch_data[index].stitch_type;
  bool sc = !memcmp(&cell_color, &color, sizeof(PatternColor));
  bool ss = !memcmp(&cell_stitch, &stitch, sizeof(StitchType));
  if (sc && ss) {
    return;
  }
  // TODO: make dynamic array.
  //----------------------------
  //  INFO: sized for potentially every cell in the grid because I cbf adding
  //  yet another dynamic array. I'll probably make it dynamic later.
  Coords *ct = calloc(pattern->width * pattern->height, sizeof(Coords));
  if (ct == NULL) {
    fprintf(stderr, "Error initializing Coord table.\n");
    return;
  }
  // make sure old memory is cleaned up if user undid things.
  // INFO: Wondering if I can walk the grid, then size the action in the group.
  // Thus not having to size it to the max of the pattern.
  if (!history_ensure_capacity(ht)) {
    free(ct);
    return;
  }
  history_truncate_future(ht);
  ActionGroup *agroup = &ht->group[ht->current_position];
  if (ht->group[ht->current_position].group_size == 0) {
    int total_cells = pattern->width * pattern->height;
    agroup->action = calloc(total_cells, sizeof(StitchDelta));
  }
  int top = 0;
  ct[top].col = col;
  ct[top].row = row;
  top++;
  agroup->action_type = ACTION_STITCH_EDIT;
  agroup->action[agroup->group_size].before_state = pattern->stitch_data[index];
  agroup->action[agroup->group_size].cell_num = index;
  pattern->stitch_data[index].stitch_color = color;
  pattern->stitch_data[index].stitch_type = stitch;
  agroup->action[agroup->group_size].after_state = pattern->stitch_data[index];
  agroup->group_size++;
  while (top > 0) {
    top--;
    int cur_r = ct[top].row;
    int cur_c = ct[top].col;

    // North check
    if (cur_r - 1 >= 0) {
      int up_idx = ((cur_r - 1) * pattern->width) + cur_c;
      bool sc = !memcmp(&pattern->stitch_data[up_idx].stitch_color, &cell_color,
                        sizeof(PatternColor));
      bool ss = !memcmp(&pattern->stitch_data[up_idx].stitch_type, &cell_stitch,
                        sizeof(StitchType));
      if (sc && ss) {
        agroup->action[agroup->group_size].before_state =
            pattern->stitch_data[up_idx];
        agroup->action[agroup->group_size].cell_num = up_idx;
        pattern->stitch_data[up_idx].stitch_color = color;
        pattern->stitch_data[up_idx].stitch_type = stitch;
        agroup->action[agroup->group_size].after_state =
            pattern->stitch_data[up_idx];
        agroup->group_size++;
        ct[top].row = cur_r - 1;
        ct[top].col = cur_c;
        top++;
      }
    }

    // South check
    if (cur_r + 1 < pattern->height) {
      int dn_idx = ((cur_r + 1) * pattern->width) + cur_c;
      bool sc = !memcmp(&pattern->stitch_data[dn_idx].stitch_color, &cell_color,
                        sizeof(PatternColor));
      bool ss = !memcmp(&pattern->stitch_data[dn_idx].stitch_type, &cell_stitch,
                        sizeof(StitchType));
      if (sc && ss) {
        agroup->action[agroup->group_size].before_state =
            pattern->stitch_data[dn_idx];
        agroup->action[agroup->group_size].cell_num = dn_idx;
        pattern->stitch_data[dn_idx].stitch_color = color;
        pattern->stitch_data[dn_idx].stitch_type = stitch;
        agroup->action[agroup->group_size].after_state =
            pattern->stitch_data[dn_idx];
        agroup->group_size++;
        ct[top].row = cur_r + 1;
        ct[top].col = cur_c;
        top++;
      }
    }

    // East check
    if (cur_c + 1 < pattern->width) {
      int rt_idx = (cur_r * pattern->width) + (cur_c + 1);
      bool sc = !memcmp(&pattern->stitch_data[rt_idx].stitch_color, &cell_color,
                        sizeof(PatternColor));
      bool ss = !memcmp(&pattern->stitch_data[rt_idx].stitch_type, &cell_stitch,
                        sizeof(StitchType));
      if (sc && ss) {
        agroup->action[agroup->group_size].before_state =
            pattern->stitch_data[rt_idx];
        agroup->action[agroup->group_size].cell_num = rt_idx;
        pattern->stitch_data[rt_idx].stitch_color = color;
        pattern->stitch_data[rt_idx].stitch_type = stitch;
        agroup->action[agroup->group_size].after_state =
            pattern->stitch_data[rt_idx];
        agroup->group_size++;
        ct[top].row = cur_r;
        ct[top].col = cur_c + 1;
        top++;
      }
    }

    // West check
    if (cur_c - 1 >= 0) {
      int lt_idx = (cur_r * pattern->width) + (cur_c - 1);
      bool sc = !memcmp(&pattern->stitch_data[lt_idx].stitch_color, &cell_color,
                        sizeof(PatternColor));
      bool ss = !memcmp(&pattern->stitch_data[lt_idx].stitch_type, &cell_stitch,
                        sizeof(StitchType));
      if (sc && ss) {
        agroup->action[agroup->group_size].before_state =
            pattern->stitch_data[lt_idx];
        agroup->action[agroup->group_size].cell_num = lt_idx;
        pattern->stitch_data[lt_idx].stitch_color = color;
        pattern->stitch_data[lt_idx].stitch_type = stitch;
        agroup->action[agroup->group_size].after_state =
            pattern->stitch_data[lt_idx];
        agroup->group_size++;
        ct[top].row = cur_r;
        ct[top].col = cur_c - 1;
        top++;
      }
    }
  }
  free(ct);
  StitchDelta *adj_size =
      realloc(agroup->action, agroup->group_size * sizeof(StitchDelta));
  if (adj_size != NULL) {
    agroup->action = adj_size;
  }
  ht->history_count++;
  ht->current_position++;
}

void pattern_reset_size(PatternData *pattern, int new_width, int new_height) {
  free(pattern->stitch_data);
  pattern->stitch_data =
      calloc((size_t)(new_width * new_height), sizeof(StitchData));
  pattern->width = new_width;
  pattern->height = new_height;
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
