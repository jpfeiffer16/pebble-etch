#include <pebble.h>
  
typedef struct DrawPoints {
  int pos_x;
  int pos_y;
} DrawPoints;

typedef struct LinePoints {
  int first_pos_x;
  int first_pos_y;
  int second_pos_x;
  int second_pos_y;
} LinePoints;

LinePoints current_line;
  
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
int prev_pos_x = 1;
int prev_pos_y = 1;

int point_counter = 1;

//DrawPoints points[2000];


int line_points_len = 1000;
LinePoints line_points[1000];

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
  point_counter++;
  line_points[point_counter].first_pos_x = line_points[point_counter - 1].second_pos_x;
  line_points[point_counter].first_pos_y = line_points[point_counter - 1].second_pos_y;
  line_points[point_counter].second_pos_x = line_points[point_counter - 1].second_pos_x;
  line_points[point_counter].second_pos_y = line_points[point_counter - 1].second_pos_y;
  vibes_short_pulse();
}

static void save_point() {
//   points[point_counter].pos_x = pos_x;
//   points[point_counter].pos_y = pos_y;
//   point_counter++;
  
}

static void update_point () {
//   line_points[point_counter];
}

static int get_first_null() {
  for (int i = 0; i <= line_points_len; i++) {
    if (line_points[i].first_pos_y == 0) {
      return i;
    }
  }
  return -1;
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch(travel_direction) {
    case 0:
//         pos_x++;
      line_points[point_counter].second_pos_x++;
      break;
    case 1:
//         pos_y--;
      line_points[point_counter].second_pos_y--;
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
//         pos_x--;
      line_points[point_counter].second_pos_x--;
      break;
    case 1:
//         pos_y++;
      line_points[point_counter].second_pos_y++;
      break;
  }
  save_point();
  layer_mark_dirty(draw_layer);
}

static void render(Layer *layer, GContext *ctx) {
//   GBitmap *draw;
//   draw = graphics_capture_frame_buffer(ctx);
  GPoint first_point;
  GPoint second_point;
//   coords.x = pos_x;
//   coords.y = pos_y;
//   GRect rect;
//   rect.size.h = 50;
//   rect.size.w = 50;
  graphics_context_set_stroke_color(ctx, GColorBlack);
//   graphics_draw_rect(ctx, rect);
  for(int i = 0; i <= point_counter; i++) {
    first_point.x = line_points[i].first_pos_x;
    first_point.y = line_points[i].first_pos_y;
    second_point.x = line_points[i].second_pos_x;
    second_point.y = line_points[i].second_pos_y;
    graphics_draw_line(ctx, first_point, second_point);
//     graphics_draw_pixel(ctx, coords);
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
  line_points[1].second_pos_x = 0;
  line_points[1].second_pos_y = 0;
  
  my_window = window_create();

  Layer *window_layer = window_get_root_layer(my_window);
  GRect bounds = layer_get_frame(window_layer);
  
  draw_layer = layer_create(bounds);
  layer_set_update_proc(draw_layer, render);
  layer_add_child(window_layer, draw_layer);
  
  window_set_click_config_provider(my_window, click_config_provider);
  
  window_stack_push(my_window, true);
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
