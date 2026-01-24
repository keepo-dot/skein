#include "pattern_data.h"
#include <gtk/gtk.h>

#ifndef TOOLBAR_STATE
#define TOOLBAR_STATE

typedef enum {
  MODE_MOVE,
  MODE_PAINT,
  MODE_ERASE,
  MODE_PICKER,
  NUM_TOOLS,
} ToolbarMode;

typedef struct {
  GtkWidget *tool_buttons[4];
  ToolbarMode active_mode;
  GdkRGBA active_color;
  StitchData active_stitch;
  GtkWidget *active_button;
} ToolbarState;

#endif // !TOOLBAR_STATE
