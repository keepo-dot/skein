#include "pattern_data.h"
#include <gtk/gtk.h>

#ifndef TOOLBAR_STATE
#define TOOLBAR_STATE

typedef enum {
  MODE_MOVE,
  MODE_PAINT,
  MODE_ERASE,
  MODE_PICKER,
  MODE_COLOR,
  MODE_STITCH,
  NUM_TOOLS,
} ToolbarMode;

typedef struct {
  GtkWidget *tool_buttons[4];
  GtkWidget *color_buttons[7];
  ToolbarMode active_mode;
  GdkRGBA active_color;
  StitchType active_stitch;
  GtkWidget *active_button;
  GtkWidget *palette_container;
  GtkWidget *stitch_type_container;
} ToolbarState;

#endif // !TOOLBAR_STATE
