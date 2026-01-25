#include <gtk/gtk.h>
#include <stdbool.h>
#ifndef PATTERN_DATA_H
#define PATTERN_DATA_H

#define STITCH_SIZE 30

typedef struct {
  char stitch_type[4]; // Type of stitch, 4 byte string ie M1L, K, P, SSK, YO.
  const GdkRGBA *stitch_color; // color value
} StitchData;

typedef struct {
  int width;               // X stitches wide.
  int height;              // Y rows high.
  StitchData *stitch_data; // What's actually IN the objects in the grid.
  bool redraw;
} PatternData;

#endif
