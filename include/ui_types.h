#ifndef UI_TYPES_H
#define UI_TYPES_H

#include <stddef.h>
#define TO_GDK_RGBA(c) ((GdkRGBA){(c).red, (c).green, (c).blue, (c).alpha})
#define TO_PATTERN_COLOR(c)                                                    \
  ((PatternColor){(c).red, (c).green, (c).blue, (c).alpha})

#include "pattern_types.h"
#include <gtk/gtk.h>

typedef enum {
  MODE_MOVE,
  MODE_PAINT,
  MODE_FILL,
  MODE_ERASE,
  MODE_PICKER,
  MODE_COLOR,
  MODE_STITCH,
  MODE_NEWFILE,
  MODE_SAVEFILE,
  MODE_LOADFILE,
  MODE_REPEAT,
  NUM_TOOLS,
} ToolbarMode;

typedef struct {
  const char *label;           // button name, default if no icon.
  const char *icon_name;       // button icon if included.
  StitchType stitch_type;      // stitch type for stitch buttons.
  ToolbarMode button_type;     // tool type for tool buttons.
  const GdkRGBA *button_color; // color value for default palette.
  gpointer type_ptr;
  bool is_toggle;
} ButtonInfo;

typedef struct {
  GtkWidget *palette_container;
  GtkWidget *stitch_type_container;
  GtkWidget *tool_container;
  GtkWidget *fileio_container;
  ToolbarMode active_mode;
  GdkRGBA active_color;
  StitchType active_stitch;
  GtkWidget *active_button;
  bool is_drawing_repeat;
} ToolbarState;

typedef struct {
  PatternData *pattern_data;
  ToolbarState *toolbar_state;
  bool redraw;          // if grid needs redrawn.
  double camera_x;      // viewport x position.
  double camera_y;      // viewport y position.
  double drag_start_x;  // drag event start x pos.
  double drag_start_y;  // drag event start y pos.
  double mouse_start_x; // mouse start x pos.
  double mouse_start_y; // mouse start y pos.
  double offset_x;
  double offset_y;
  int temp_repeat_start;
  int temp_repeat_end;
  GdkPixbuf *repeat_icon;
  size_t hovered_repeat_index;
} UiState;

typedef struct {
  PatternData *pattern;
  UiState *ui;
  GtkWidget *main_window;
} AppState;

#endif // !UI_TYPES_H
