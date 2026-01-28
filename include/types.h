#include <gtk/gtk.h>
#ifndef TYPES_H
#define TYPES_H

#define STITCH_SIZE 30

typedef enum {
  STITCH_EMPTY = 0,
  STITCH_KNIT,
  STITCH_PURL,
  STITCH_YO,
  STITCH_K2TOG,
  STITCH_SSK,
  NUM_STITCH_TOOLS,
} StitchType;

typedef enum {
  MODE_MOVE,
  MODE_PAINT,
  MODE_ERASE,
  MODE_PICKER,
  MODE_COLOR,
  MODE_STITCH,
  MODE_NEWFILE,
  MODE_SAVEFILE,
  MODE_LOADFILE,
  NUM_TOOLS,
} ToolbarMode;

typedef struct {
  GdkRGBA stitch_color;   // color value
  StitchType stitch_type; // type of stitch.
} StitchData;

typedef struct {
  int width;               // X stitches wide.
  int height;              // Y rows high.
  StitchData *stitch_data; // What's actually IN the objects in the grid.
  bool redraw;             // if grid needs redrawn.
  double camera_x;         // viewport x position.
  double camera_y;         // viewport y position.
  double drag_start_x;     // drag event start x pos.
  double drag_start_y;     // drag event start y pos.
  double mouse_start_x;    // mouse start x pos.
  double mouse_start_y;    // mouse start y pos.
} PatternData;

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
} ToolbarState;

typedef struct {
  PatternData *pattern_data;
  ToolbarState *toolbar_state;
  double offset_x;
  double offset_y;
} UiState;

typedef struct {
  PatternData *pattern;
  UiState *ui;
} AppState;

#endif // !TYPES_H
