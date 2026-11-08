#ifndef PATTERN_H
#define PATTERN_H

#include "pattern_types.h"
#include <stdbool.h>

// Cell mutations and drawing
void pattern_apply_stitch(PatternData *pattern, int index, PatternColor color,
                          StitchType stitch);
void pattern_apply_stitch_drag(PatternData *pattern, int index,
                               PatternColor color, StitchType stitch);
void pattern_apply_stitch_end(PatternData *pattern);
void pattern_apply_fill(PatternData *pattern, int col, int row,
                        PatternColor color, StitchType stitch);

// Grid lifecycle and queries
void pattern_reset_size(PatternData *pattern, int new_width, int new_height);
bool is_pattern_empty(PatternData *pattern);

// Model-level history transactions
bool pattern_undo(PatternData *pattern);
bool pattern_redo(PatternData *pattern);

// Repeat section domain mutations
void pattern_add_repeat(PatternData *pattern, int start_row, int end_row,
                        char *repeat_info);

#endif // !PATTERN_H
