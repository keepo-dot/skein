#ifndef PATTERN_DATA_H
#define PATTERN_DATA_H

#define STITCH_SIZE 30

typedef struct {
  int type;     // Type of stitvh, 1 for knit, 2 for purl, etc.
  int color_id; // color value, preferably hex in the future.
} Stitch;

typedef struct {
  int width;         // X stitches wide.
  int height;        // Y rows high.
  Stitch *grid_data; // What's actually IN the objects in the grid.
} PatternData;

#endif
