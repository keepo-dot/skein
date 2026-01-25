#include <gtk/gtk.h>
#include <stdbool.h>
#ifndef PATTERN_DATA_H
#define PATTERN_DATA_H

#define STITCH_SIZE 30

typedef struct {
  char stitch_type[8];  // Type of stitch, 4 byte string ie M1L, K, P, SSK, YO.
  GdkRGBA stitch_color; // color value
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
} PatternData;

#endif
