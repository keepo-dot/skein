#ifndef PATTERN_TYPES_H
#define PATTERN_TYPES_H
#include <stddef.h>

typedef enum {
  STITCH_EMPTY = 0,
  STITCH_KNIT,
  STITCH_PURL,
  STITCH_M1,
  STITCH_SLIP,
  STITCH_YO,
  STITCH_K2TOG,
  STITCH_SSK,
  NUM_STITCH_TOOLS,
} StitchType;

typedef enum {
  ACTION_NONE,
  ACTION_STITCH_EDIT,
  ACTION_REPEAT_ADD,
} ActionType;

typedef struct {
  int col;
  int row;
} Coords;

typedef struct {
  float red;
  float green;
  float blue;
  float alpha;
} PatternColor;

typedef struct {
  PatternColor stitch_color; // color value
  StitchType stitch_type;    // type of stitch.
} StitchData;

typedef struct {
  int cell_num;
  StitchData before_state;
  StitchData after_state;
} StitchDelta;

typedef struct {
  size_t group_size;
  StitchDelta *action;
  ActionType action_type;
} ActionGroup;

typedef struct {
  size_t table_size;
  ActionGroup *group;
  size_t current_position;
  size_t history_count;
} HistoryTable;

typedef struct {
  int start_row;
  int end_row;
  char *repeat_info;
} RepeatSection;

typedef struct {
  size_t table_size;
  size_t num_repeats;
  RepeatSection *repeat_section;
} RepeatTable;

typedef struct {
  int width;               // X stitches wide.
  int height;              // Y rows high.
  StitchData *stitch_data; // What's actually IN the objects in the grid.
  int stitch_size;
  HistoryTable *history_table;
  RepeatTable *repeat_table;
} PatternData;

#endif // !PATTERN_TYPES_H
