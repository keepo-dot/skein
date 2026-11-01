#include "pattern_types.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void pattern_apply_stitch(PatternData *pattern, int index, PatternColor color,
                          StitchType stitch) {
  HistoryTable *history_table = pattern->history_table;
  // If the current history table position is less than the total count, wipe
  // the history ahead so new actions replace the old future.
  if (history_table->current_position < history_table->history_count) {
    for (size_t i = history_table->current_position;
         i < history_table->history_count; i++) {
      free(history_table->group[i].action);
      history_table->group[i].action = NULL;
      history_table->group[i].group_size = 0;
    }
    history_table->history_count = history_table->current_position;
  }
  // Resize the history table if it's full.
  if (history_table->table_size == history_table->current_position) {
    size_t new_cap = history_table->table_size * 2;
    if (!new_cap) {
      new_cap = 1;
    }
    ActionGroup *new_group =
        realloc(history_table->group, new_cap * sizeof(ActionGroup));
    if (new_group == NULL) {
      // TODO: add error handling a la Go. I like error handling. Until then
      // just print to console.
      fprintf(stderr, "Error: Failed to allocate memory for history group.\n");
      return;
    }
    history_table->group = new_group;

    for (size_t i = history_table->table_size; i < new_cap; i++) {
      history_table->group[i].group_size = 0;
      history_table->group[i].action = NULL;
      history_table->group[i].action_type = 0;
    }
    history_table->table_size = new_cap;
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
