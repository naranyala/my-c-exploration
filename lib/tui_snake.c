#define TUI_IMPLEMENTATION
#include "tui.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SNAKE_LEN 500
#define INITIAL_SNAKE_LEN 4
#define GAME_SPEED 100 // milliseconds

typedef struct {
  int y, x;
} Point;

typedef struct {
  Point body[MAX_SNAKE_LEN];
  int length;
  int dy, dx; // direction
} Snake;

typedef struct {
  Point pos;
  int active;
} Food;

typedef enum { DIR_UP = 0, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

// Color pairs
#define COLOR_SNAKE 1
#define COLOR_FOOD 2
#define COLOR_BORDER 3
#define COLOR_SCORE 4

int game_rows, game_cols;
int score = 0;
int game_over = 0;

void init_snake(Snake *snake, int rows, int cols) {
  snake->length = INITIAL_SNAKE_LEN;
  snake->dy = 0;
  snake->dx = 1; // moving right initially

  int start_y = rows / 2;
  int start_x = cols / 2;

  for (int i = 0; i < snake->length; i++) {
    snake->body[i].y = start_y;
    snake->body[i].x = start_x - i;
  }
}

void spawn_food(Food *food, Snake *snake, int rows, int cols) {
  int valid = 0;

  while (!valid) {
    food->pos.y = 2 + rand() % (rows - 4);
    food->pos.x = 2 + rand() % (cols - 4);

    valid = 1;
    // Check if food spawns on snake
    for (int i = 0; i < snake->length; i++) {
      if (snake->body[i].y == food->pos.y && snake->body[i].x == food->pos.x) {
        valid = 0;
        break;
      }
    }
  }

  food->active = 1;
}

int check_collision(Snake *snake, int rows, int cols) {
  Point head = snake->body[0];

  // Wall collision
  if (head.y <= 1 || head.y >= rows - 2 || head.x <= 1 || head.x >= cols - 2) {
    return 1;
  }

  // Self collision
  for (int i = 1; i < snake->length; i++) {
    if (head.y == snake->body[i].y && head.x == snake->body[i].x) {
      return 1;
    }
  }

  return 0;
}

void move_snake(Snake *snake, Food *food, int rows, int cols) {
  Point new_head;
  new_head.y = snake->body[0].y + snake->dy;
  new_head.x = snake->body[0].x + snake->dx;

  // Check if food is eaten
  int ate_food = 0;
  if (food->active && new_head.y == food->pos.y && new_head.x == food->pos.x) {
    ate_food = 1;
    score += 10;
    food->active = 0;
  }

  // Move body
  if (!ate_food) {
    for (int i = snake->length - 1; i > 0; i--) {
      snake->body[i] = snake->body[i - 1];
    }
  } else {
    // Grow snake
    for (int i = snake->length; i > 0; i--) {
      snake->body[i] = snake->body[i - 1];
    }
    snake->length++;
  }

  snake->body[0] = new_head;
}

void change_direction(Snake *snake, int key) {
  switch (key) {
  case KEY_UP:
  case 'w':
  case 'W':
    if (snake->dy != 1) { // Can't go opposite direction
      snake->dy = -1;
      snake->dx = 0;
    }
    break;
  case KEY_DOWN:
  case 's':
  case 'S':
    if (snake->dy != -1) {
      snake->dy = 1;
      snake->dx = 0;
    }
    break;
  case KEY_LEFT:
  case 'a':
  case 'A':
    if (snake->dx != 1) {
      snake->dy = 0;
      snake->dx = -1;
    }
    break;
  case KEY_RIGHT:
  case 'd':
  case 'D':
    if (snake->dx != -1) {
      snake->dy = 0;
      snake->dx = 1;
    }
    break;
  }
}

void draw_game(Snake *snake, Food *food, int rows, int cols) {
  tui_clear();

  // Draw border
  tui_set_color(COLOR_BORDER);
  tui_draw_box(1, 1, rows - 2, cols - 2);

  // Draw score
  tui_set_color(COLOR_SCORE);
  tui_attr_on(A_BOLD);
  tui_print(0, 2, " SNAKE GAME ");
  tui_print(0, cols - 20, " Score: %d ", score);
  tui_print(rows - 1, 2, " WASD/Arrows: Move | Q: Quit ");
  tui_attr_off(A_BOLD);

  // Draw food
  if (food->active) {
    tui_set_color(COLOR_FOOD);
    tui_attr_on(A_BOLD);
    tui_print(food->pos.y, food->pos.x, "@");
    tui_attr_off(A_BOLD);
  }

  // Draw snake
  tui_set_color(COLOR_SNAKE);
  for (int i = 0; i < snake->length; i++) {
    if (i == 0) {
      tui_attr_on(A_BOLD);
      tui_print(snake->body[i].y, snake->body[i].x, "O");
      tui_attr_off(A_BOLD);
    } else {
      tui_print(snake->body[i].y, snake->body[i].x, "o");
    }
  }

  tui_reset_color();
  tui_refresh();
}

void show_game_over(int rows, int cols) {
  tui_clear();

  int msg_y = rows / 2 - 2;
  int msg_x = cols / 2;

  tui_set_color(COLOR_FOOD);
  tui_attr_on(A_BOLD);
  tui_print(msg_y, msg_x - 6, "GAME OVER!");
  tui_attr_off(A_BOLD);

  tui_reset_color();
  tui_print(msg_y + 2, msg_x - 10, "Final Score: %d", score);
  tui_print(msg_y + 3, msg_x - 15, "Press any key to exit...");

  tui_refresh();
  tui_set_timeout(-1); // Blocking input
  tui_getch();
}

int main() {
  srand(time(NULL));

  tui_init();
  tui_set_cursor(0); // Hide cursor
  tui_get_size(&game_rows, &game_cols);

  // Initialize colors
  tui_init_color_pair(COLOR_SNAKE, COLOR_GREEN, -1);
  tui_init_color_pair(COLOR_FOOD, COLOR_RED, -1);
  tui_init_color_pair(COLOR_BORDER, COLOR_CYAN, -1);
  tui_init_color_pair(COLOR_SCORE, COLOR_YELLOW, -1);

  Snake snake;
  Food food;

  init_snake(&snake, game_rows, game_cols);
  spawn_food(&food, &snake, game_rows, game_cols);

  tui_set_timeout(GAME_SPEED);

  // Game loop
  while (!game_over) {
    draw_game(&snake, &food, game_rows, game_cols);

    int key = tui_getch();

    if (key == 'q' || key == 'Q' || key == 27) { // ESC or Q to quit
      break;
    }

    if (key != ERR) {
      change_direction(&snake, key);
    }

    move_snake(&snake, &food, game_rows, game_cols);

    if (check_collision(&snake, game_rows, game_cols)) {
      game_over = 1;
    }

    if (!food.active) {
      spawn_food(&food, &snake, game_rows, game_cols);
    }
  }

  show_game_over(game_rows, game_cols);

  tui_end();

  printf("\nThanks for playing! Final score: %d\n", score);

  return 0;
}
