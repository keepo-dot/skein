#include "pattern_data.h"
#include "ui_state.h"
#ifndef APP_STATE
#define APP_STATE

typedef struct {
  PatternData *pattern;
  UiState *ui;
} AppState;

#endif // !APP_STATE
