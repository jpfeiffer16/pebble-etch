#include <pebble.h>

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 0
#define NUM_MENU_ITEMS 2
  
#define NUM_FIRST_MENU_ITEMS 3
#define NUM_SECOND_MENU_ITEMS 1
  
typedef struct draw_points{
  int pos_x;
  int pos_y;
} draw_points;
  
Window *my_window;
Window *menu_window;
TextLayer *text_layer;
Layer *draw_layer;
BitmapLayer *motion_layer;
MenuLayer *main_menu;

GBitmap *bitmap;

int travel_direction = 0;

int pos_x = 1;

int pos_y = 1;

int point_counter = 0;

draw_points points[1000];


static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return NUM_MENU_ITEMS;
//   switch (section_index) {
//     case 0:
//       return NUM_FIRST_MENU_ITEMS;
//     case 1:
//       return NUM_SECOND_MENU_ITEMS;
//     default:
//       return 0;
//   }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Yo, Check it..");
      break;
    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "One more");
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          menu_cell_title_draw(ctx, cell_layer, "Horizontal");
          break;
        case 1:
          menu_cell_title_draw(ctx, cell_layer, "Vertical");
          break;
      }
      break;
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  char* value = "";
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
    case 0:
      travel_direction = 0;
      break;
    case 1:
      travel_direction = 1;
      break;
  }
  char* complete_value;
  complete_value = "You pressed:";
  //strcpy(complete_value, value);
  strcat(complete_value, value);
  text_layer_set_text(text_layer, complete_value);
  window_stack_pop(true);
  menu_layer_destroy(main_menu);
  window_destroy(menu_window);
  vibes_short_pulse();
}

static void setup_menu_basic(Layer *this_layer, GRect bounds) {
//   MenuLayer* this_layer;
  main_menu = menu_layer_create(bounds);
  menu_layer_set_callbacks(main_menu, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  
  menu_layer_set_click_config_onto_window(main_menu, menu_window);

  layer_add_child(this_layer, menu_layer_get_layer(main_menu));
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
//   menu_window = window_create();
//   window_stack_push(menu_window, true);
//   Layer *window_layer = window_get_root_layer(menu_window);
//   GRect bounds = layer_get_frame(window_layer);
//   setup_menu_basic(window_layer, bounds);
  if(travel_direction == 0) {
    travel_direction = 1;
  } else {
    travel_direction = 0;
  }
  vibes_short_pulse();
}

static void save_point() {
  points[point_counter].pos_x = pos_x;
  points[point_counter].pos_y = pos_y;
  point_counter++;
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch(travel_direction) {
    case 0:
        pos_x++;
      break;
    case 1:
        pos_y--;
      break;
  }
  save_point();
  layer_mark_dirty(draw_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch(travel_direction) {
    case 0:
//       siprintf(status, "%g", (double)pos_x);
//       text_layer_set_text(text_layer, status);
        pos_x--;
      break;
    case 1:
        pos_y++;
      break;
  }
  save_point();
  layer_mark_dirty(draw_layer);
}

static void render(Layer *layer, GContext *ctx) {
//   GBitmap *draw;
//   draw = graphics_capture_frame_buffer(ctx);
  GPoint coords;
//   coords.x = pos_x;
//   coords.y = pos_y;
//   GRect rect;
//   rect.size.h = 50;
//   rect.size.w = 50;
  graphics_context_set_stroke_color(ctx, GColorBlack);
//   graphics_draw_rect(ctx, rect);
  for(int i = 0; i < 1000; i++) {
    coords.x = points[i].pos_x;
    coords.y = points[i].pos_y;
    graphics_draw_pixel(ctx, coords);
  }
//   graphics_draw_pixel(ctx, coords);
//   graphics_release_frame_buffer(ctx, draw);
//   app_log(1, "This", 2, "%g", 2.0);
}

static void click_config_provider(void *context) {
  static uint16_t  interval = 50;
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, interval, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, interval, down_click_handler);
}

void handle_init(void) {
  my_window = window_create();

  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_frame(window_layer);
  
//   bitmap = gbitmap_create_with_resource(RESOURCE_ID_BUILDINGS_ICON);
//   motion_layer = bitmap_layer_create(bounds);
//   bitmap_layer_set_bitmap(motion_layer, bitmap);
//   layer_add_child(window_layer, bitmap_layer_get_layer(motion_layer));
  
  
  
  
//   text_layer = text_layer_create(GRect(0, 0, 144, 20));
//   text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
//   text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
//   text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
//   layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  draw_layer = layer_create(bounds);
  layer_set_update_proc(draw_layer, render);
  layer_add_child(window_layer, draw_layer);
  
  window_set_click_config_provider(my_window, click_config_provider);
  
  
  
  window_stack_push(my_window, true);
  
  // Now we prepare to initialize the menu layer
  

  // Create the menu layer
//   setup_menu_basic(window_layer, bounds);
  
//   main_menu = menu_layer_create(bounds);
//   menu_layer_set_callbacks(main_menu, NULL, (MenuLayerCallbacks){
//     .get_num_sections = menu_get_num_sections_callback,
//     .get_num_rows = menu_get_num_rows_callback,
//     .get_header_height = menu_get_header_height_callback,
//     .draw_header = menu_draw_header_callback,
//     .draw_row = menu_draw_row_callback,
//     .select_click = menu_select_callback,
//   });
  
//   menu_layer_set_click_config_onto_window(main_menu, my_window);

//   layer_add_child(window_layer, menu_layer_get_layer(main_menu));
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
