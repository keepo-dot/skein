#include "pattern_types.h"

void pattern_apply_stitch(PatternData *pattern, int index, PatternColor color,
                          StitchType stitch);
void pattern_apply_stitch_drag(PatternData *pattern, int index,
                               PatternColor color, StitchType stitch);

void pattern_apply_stitch_end(PatternData *pattern);

void pattern_apply_fill(PatternData *pattern, int index, PatternColor color,
                        StitchType stitch);
