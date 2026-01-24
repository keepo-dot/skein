#include "pattern_data.h"
#include "toolbar_state.h"

#ifndef UI_STATE
#define UI_STATE

typedef struct {
  PatternData *pattern_data;
  ToolbarState *toolbar_state;
  double offset_x;
  double offset_y;
} UiState;
#endif // !UI_STATE
