#include "resources.h"
#include "types.h"
#include <gtk/gtk.h>
#include <json-glib-1.0/json-glib/json-glib.h>
#include <stddef.h>

void pattern_json_load(char *filename, PatternData *current_canvas) {
  GError *error = NULL;
  JsonParser *parser = json_parser_new();
  json_parser_load_from_file(parser, filename, &error);
  if (error) {
    g_print("Unable to load file '%s': %s\n", filename, error->message);
    g_error_free(error);
    return;
  }
  JsonNode *root = json_parser_get_root(parser);
  JsonReader *reader = json_reader_new(root);

  json_reader_read_member(reader, "pattern_width");
  int new_width = json_reader_get_int_value(reader);
  json_reader_end_member(reader);

  json_reader_read_member(reader, "pattern_height");
  int new_height = json_reader_get_int_value(reader);
  json_reader_end_member(reader);

  int num_cells = new_width * new_height;

  if (new_width != current_canvas->width ||
      new_height != current_canvas->height) {
    free(current_canvas->stitch_data);
    current_canvas->stitch_data = calloc((size_t)num_cells, sizeof(StitchData));
    current_canvas->width = new_width;
    current_canvas->height = new_height;
  } else {
    // wipe memory so old stitches don't show up in the new canvas
    memset(current_canvas->stitch_data, 0,
           (size_t)num_cells * sizeof(StitchData));
  }
  json_reader_read_member(reader, "stitch_data");
  int num_elements = json_reader_count_elements(reader);
  int canvas_index = 0;
  for (int i = 0; i < num_elements; i++) {
    json_reader_read_element(reader, i);
    if (json_reader_read_member(reader, "zero_stitch_count")) {
      int skip_count = json_reader_get_int_value(reader);
      canvas_index += skip_count;
      json_reader_end_member(reader);

    } else {
      json_reader_end_member(reader);
      json_reader_read_member(reader, "stitch_type");
      int type = json_reader_get_int_value(reader);
      json_reader_end_member(reader);
      json_reader_read_member(reader, "stitch_color");
      json_reader_read_element(reader, 0);
      double r = json_reader_get_double_value(reader);
      json_reader_end_element(reader);
      json_reader_read_element(reader, 1);
      double g = json_reader_get_double_value(reader);
      json_reader_end_element(reader);
      json_reader_read_element(reader, 2);
      double b = json_reader_get_double_value(reader);
      json_reader_end_element(reader);
      json_reader_read_element(reader, 3);
      double a = json_reader_get_double_value(reader);
      json_reader_end_element(reader);
      json_reader_end_member(reader);
      // check if it has duplicates
      int duplicates = 1;

      if (json_reader_read_member(reader, "duplicate_count")) {
        duplicates = json_reader_get_int_value(reader);
        json_reader_end_member(reader);
      } else {
        json_reader_end_member(reader);
      }

      for (int d = 0; d < duplicates && canvas_index < num_cells; d++) {
        current_canvas->stitch_data[canvas_index].stitch_type = type;
        current_canvas->stitch_data[canvas_index].stitch_color.red = r;
        current_canvas->stitch_data[canvas_index].stitch_color.green = g;
        current_canvas->stitch_data[canvas_index].stitch_color.blue = b;
        current_canvas->stitch_data[canvas_index].stitch_color.alpha = a;
        canvas_index++;
      }
    }

    json_reader_end_element(reader);
  }

  json_reader_end_member(reader);

  current_canvas->redraw = true;
  g_object_unref(reader);
  g_object_unref(parser);
}

JsonBuilder *pattern_json_builder(PatternData *pattern) {
  JsonBuilder *builder = json_builder_new();

  json_builder_begin_object(builder);
  json_builder_set_member_name(builder, "author");
  json_builder_add_string_value(builder, "keepo-dot");
  json_builder_set_member_name(builder, "revision");
  json_builder_add_int_value(builder, 1);
  json_builder_set_member_name(builder, "pattern_width");
  json_builder_add_int_value(builder, pattern->width);
  json_builder_set_member_name(builder, "pattern_height");
  json_builder_add_int_value(builder, pattern->height);
  json_builder_set_member_name(builder, "stitch_data");
  json_builder_begin_array(builder);

  int zero_count = 0;
  int duplicate_count = 0;
  StitchData *tracked_stitch = NULL;

  for (size_t i = 0; i < (size_t)(pattern->width * pattern->height); i++) {
    StitchData *current_stitch = &pattern->stitch_data[i];

    if (current_stitch->stitch_type == 0) {

      // set colored stitch data if previous run wasn't empty stitches.
      if (duplicate_count > 0 && tracked_stitch != NULL) {
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "stitch_type");
        json_builder_add_int_value(builder, tracked_stitch->stitch_type);

        json_builder_set_member_name(builder, "stitch_color");
        json_builder_begin_array(builder);
        json_builder_add_double_value(builder,
                                      tracked_stitch->stitch_color.red);
        json_builder_add_double_value(builder,
                                      tracked_stitch->stitch_color.green);
        json_builder_add_double_value(builder,
                                      tracked_stitch->stitch_color.blue);
        json_builder_add_double_value(builder,
                                      tracked_stitch->stitch_color.alpha);
        json_builder_end_array(builder);

        if (duplicate_count > 1) {
          json_builder_set_member_name(builder, "duplicate_count");
          json_builder_add_int_value(builder, duplicate_count);
        }
        json_builder_end_object(builder);

        tracked_stitch = NULL;
        duplicate_count = 0;
      }
      zero_count++;

    } else {
      // if not a zero stitch, run out zeros.
      if (zero_count > 0) {
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "zero_stitch_count");
        json_builder_add_int_value(builder, zero_count);
        json_builder_end_object(builder);
        zero_count = 0;
      }

      // track dupes
      if (tracked_stitch != NULL) {
        if (memcmp(current_stitch, tracked_stitch, sizeof(StitchData)) == 0) {
          duplicate_count++;
        } else {
          // current stitch is different, so close out the old set of dupes
          json_builder_begin_object(builder);
          json_builder_set_member_name(builder, "stitch_type");
          json_builder_add_int_value(builder, tracked_stitch->stitch_type);

          json_builder_set_member_name(builder, "stitch_color");
          json_builder_begin_array(builder);
          json_builder_add_double_value(builder,
                                        tracked_stitch->stitch_color.red);
          json_builder_add_double_value(builder,
                                        tracked_stitch->stitch_color.green);
          json_builder_add_double_value(builder,
                                        tracked_stitch->stitch_color.blue);
          json_builder_add_double_value(builder,
                                        tracked_stitch->stitch_color.alpha);
          json_builder_end_array(builder);

          if (duplicate_count > 1) {
            json_builder_set_member_name(builder, "duplicate_count");
            json_builder_add_int_value(builder, duplicate_count);
          }
          json_builder_end_object(builder);

          // track next stitch
          tracked_stitch = current_stitch;
          duplicate_count = 1;
        }
      } else {
        // otherwise we're not tracking anything, track it.
        tracked_stitch = current_stitch;
        duplicate_count = 1;
      }
    }
  }

  // everything is done, set final data.
  if (zero_count > 0) {
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "zero_stitch_count");
    json_builder_add_int_value(builder, zero_count);
    json_builder_end_object(builder);

  } else if (duplicate_count > 0 && tracked_stitch != NULL) {
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "stitch_type");
    json_builder_add_int_value(builder, tracked_stitch->stitch_type);

    json_builder_set_member_name(builder, "stitch_color");
    json_builder_begin_array(builder);
    json_builder_add_double_value(builder, tracked_stitch->stitch_color.red);
    json_builder_add_double_value(builder, tracked_stitch->stitch_color.green);
    json_builder_add_double_value(builder, tracked_stitch->stitch_color.blue);
    json_builder_add_double_value(builder, tracked_stitch->stitch_color.alpha);
    json_builder_end_array(builder);

    if (duplicate_count > 1) {
      json_builder_set_member_name(builder, "duplicate_count");
      json_builder_add_int_value(builder, duplicate_count);
    }
    json_builder_end_object(builder);
  }
  json_builder_end_array(builder);
  json_builder_end_object(builder);

  return builder;
}

void pattern_json_save(JsonBuilder *builder, char *file_path) {
  JsonNode *root = json_builder_get_root(builder);
  JsonGenerator *generator = json_generator_new();
  json_generator_set_root(generator, root);
  json_generator_set_pretty(generator, true);
  json_generator_to_file(generator, file_path, false);
}
