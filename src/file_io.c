#include "resources.h"
#include "types.h"
#include <gtk/gtk.h>
#include <json-glib-1.0/json-glib/json-glib.h>

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

  if (new_width != current_canvas->width ||
      new_height != current_canvas->height) {
    free(current_canvas->stitch_data);
    current_canvas->stitch_data =
        calloc((size_t)(new_width * new_height), sizeof(StitchData));
    current_canvas->width = new_width;
    current_canvas->height = new_height;
  }
  json_reader_read_member(reader, "stitch_data");
  int index = 0;
  int num_cells = (new_width * new_height);
  while (index < num_cells) {
    json_reader_read_element(reader, index);
    json_reader_read_member(reader, "stitch_type");
    current_canvas->stitch_data[index].stitch_type =
        json_reader_get_int_value(reader);
    json_reader_end_member(reader);

    json_reader_read_member(reader, "stitch_color");
    json_reader_read_element(reader, 0);
    current_canvas->stitch_data[index].stitch_color.red =
        json_reader_get_double_value(reader);
    json_reader_end_element(reader);
    json_reader_read_element(reader, 1);
    current_canvas->stitch_data[index].stitch_color.green =
        json_reader_get_double_value(reader);
    json_reader_end_element(reader);
    json_reader_read_element(reader, 2);
    current_canvas->stitch_data[index].stitch_color.blue =
        json_reader_get_double_value(reader);
    json_reader_end_element(reader);
    json_reader_read_element(reader, 3);
    current_canvas->stitch_data[index].stitch_color.alpha =
        json_reader_get_double_value(reader);
    json_reader_end_element(reader);
    json_reader_end_member(reader);
    json_reader_end_element(reader);
    index++;
  }
  json_reader_end_member(reader);
  current_canvas->redraw = true;
  g_object_unref(reader);
  g_object_unref(parser);
  return;
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

  for (size_t i = 0; i < (size_t)(pattern->width * pattern->height); i++) {
    StitchData *current_stitch = &pattern->stitch_data[i];
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "stitch_type");
    json_builder_add_int_value(builder, current_stitch->stitch_type);
    json_builder_set_member_name(builder, "stitch_color");
    json_builder_begin_array(builder);
    json_builder_add_double_value(builder, current_stitch->stitch_color.red);
    json_builder_add_double_value(builder, current_stitch->stitch_color.green);
    json_builder_add_double_value(builder, current_stitch->stitch_color.blue);
    json_builder_add_double_value(builder, current_stitch->stitch_color.alpha);
    json_builder_end_array(builder);
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
