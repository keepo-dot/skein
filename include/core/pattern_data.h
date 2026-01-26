#include <gtk/gtk.h>
#include <stdbool.h>
#ifndef PATTERN_DATA_H
#define PATTERN_DATA_H

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

typedef struct {
  GdkRGBA stitch_color; // color value
  StitchType stitch_type;
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

#endif
