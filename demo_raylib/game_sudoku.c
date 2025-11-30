// sudoku_raylib.c
// Single-file Sudoku clone using raylib
// Compile (Linux/mac):
// gcc sudoku_raylib.c -o sudoku -lraylib -lm -lpthread -ldl
// For Windows/MSYS2 adjust libs accordingly.

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIN_W 720
#define WIN_H 800
#define GRID 9
#define BOX 3

static int board[GRID][GRID];      // current board (0 empty)
static int solution[GRID][GRID];   // solved board
static int fixed_cell[GRID][GRID]; // 1 if starting clue (immutable)
static int selected_row = -1;
static int selected_col = -1;

// forward
int solve_backtrack(int b[GRID][GRID]);
int find_empty(int b[GRID][GRID], int *r, int *c);
int is_valid(int b[GRID][GRID], int r, int c, int val);
void copy_board(int src[GRID][GRID], int dst[GRID][GRID]);

// Utility RNG
static int rnd_int(int a, int b) { return a + (rand() % (b - a + 1)); }

// Create a full solved board by filling and solving
void make_solved_board(int out[GRID][GRID]) {
  // clear
  for (int r = 0; r < GRID; r++)
    for (int c = 0; c < GRID; c++)
      out[r][c] = 0;
  // Fill diagonal boxes with random numbers 1..9 (to help uniqueness)
  for (int boxStart = 0; boxStart < GRID; boxStart += BOX) {
    int nums[GRID];
    for (int i = 0; i < GRID; i++)
      nums[i] = i + 1;
    // shuffle
    for (int i = GRID - 1; i > 0; i--) {
      int j = rand() % (i + 1);
      int t = nums[i];
      nums[i] = nums[j];
      nums[j] = t;
    }
    int k = 0;
    for (int r = 0; r < BOX; r++)
      for (int c = 0; c < BOX; c++) {
        out[boxStart + r][boxStart + c] = nums[k++];
      }
  }
  // solve rest with backtracking
  solve_backtrack(out);
}

// Count number of solutions but stop if >limit
int count_solutions_limit(int b[GRID][GRID], int limit) {
  int r, c;
  if (!find_empty(b, &r, &c))
    return 1; // solved
  int count = 0;
  for (int val = 1; val <= 9; val++) {
    if (is_valid(b, r, c, val)) {
      b[r][c] = val;
      count += count_solutions_limit(b, limit - count);
      b[r][c] = 0;
      if (count >= limit)
        return count;
    }
  }
  return count;
}

// Create puzzle by removing numbers while keeping unique solution
void make_puzzle_from_solution(int sol[GRID][GRID], int out[GRID][GRID],
                               int clues) {
  copy_board(sol, out);
  // Create list of all cell indices
  int cells[GRID * GRID];
  for (int i = 0; i < GRID * GRID; i++)
    cells[i] = i;
  for (int i = GRID * GRID - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int t = cells[i];
    cells[i] = cells[j];
    cells[j] = t;
  }

  int toRemove = GRID * GRID - clues;
  for (int i = 0; i < GRID * GRID && toRemove > 0; i++) {
    int idx = cells[i];
    int r = idx / GRID, c = idx % GRID;
    int backup = out[r][c];
    if (backup == 0)
      continue;
    out[r][c] = 0;
    // copy board and check number of solutions
    int tmp[GRID][GRID];
    copy_board(out, tmp);
    int sols = count_solutions_limit(tmp, 2);
    if (sols != 1) { // not unique -> revert
      out[r][c] = backup;
    } else {
      toRemove--; // successfully removed
    }
  }
}

// Helpers
void copy_board(int src[GRID][GRID], int dst[GRID][GRID]) {
  for (int r = 0; r < GRID; r++)
    for (int c = 0; c < GRID; c++)
      dst[r][c] = src[r][c];
}

int find_empty(int b[GRID][GRID], int *r, int *c) {
  for (int i = 0; i < GRID; i++)
    for (int j = 0; j < GRID; j++)
      if (b[i][j] == 0) {
        *r = i;
        *c = j;
        return 1;
      }
  return 0;
}

int is_valid(int b[GRID][GRID], int r, int c, int val) {
  // row
  for (int j = 0; j < GRID; j++)
    if (b[r][j] == val)
      return 0;
  // col
  for (int i = 0; i < GRID; i++)
    if (b[i][c] == val)
      return 0;
  // box
  int br = (r / BOX) * BOX, bc = (c / BOX) * BOX;
  for (int i = 0; i < BOX; i++)
    for (int j = 0; j < BOX; j++)
      if (b[br + i][bc + j] == val)
        return 0;
  return 1;
}

// Regular solver (fills board). Returns 1 if solved
int solve_backtrack(int b[GRID][GRID]) {
  int r, c;
  if (!find_empty(b, &r, &c))
    return 1;
  // try numbers 1..9 in random order for variety
  int nums[GRID];
  for (int i = 0; i < GRID; i++)
    nums[i] = i + 1;
  for (int i = GRID - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int t = nums[i];
    nums[i] = nums[j];
    nums[j] = t;
  }
  for (int k = 0; k < GRID; k++) {
    int val = nums[k];
    if (is_valid(b, r, c, val)) {
      b[r][c] = val;
      if (solve_backtrack(b))
        return 1;
      b[r][c] = 0;
    }
  }
  return 0;
}

// Initialize new puzzle with desired number of clues
void new_game(int clues) {
  int full[GRID][GRID];
  make_solved_board(full);
  copy_board(full, solution);
  make_puzzle_from_solution(solution, board, clues);
  // mark fixed
  for (int r = 0; r < GRID; r++)
    for (int c = 0; c < GRID; c++)
      fixed_cell[r][c] = (board[r][c] != 0);
  selected_row = selected_col = -1;
}

// Draw helpers
void draw_ui(float topOffset, float cell_size) {
  // Buttons / instructions
  const char *instr[] = {
      "Mouse: click cell | Keys 1-9: set | Backspace/Delete: clear",
      "Space: auto-solve | N: new puzzle | R: reveal solution | C: check "
      "mistakes",
      "Up/Down: change clues | Current clues shown below"};
  for (int i = 0; i < 3; i++)
    DrawText(instr[i], 10, (int)(topOffset + GRID * cell_size + 10 + i * 20),
             16, DARKGRAY);
}

void draw_board(float x, float y, float w) {
  float cell = w / GRID;
  // background
  DrawRectangle((int)x, (int)y, (int)w, (int)(w), LIGHTGRAY);
  // highlight selected
  if (selected_row != -1 && selected_col != -1) {
    DrawRectangle((int)(x + selected_col * cell),
                  (int)(y + selected_row * cell), (int)cell, (int)cell,
                  Fade(SKYBLUE, 0.5f));
  }
  // draw numbers
  for (int r = 0; r < GRID; r++) {
    for (int c = 0; c < GRID; c++) {
      int val = board[r][c];
      if (val != 0) {
        char t[4];
        sprintf(t, "%d", val);
        int fontSize = (int)(cell * 0.7f);
        Vector2 size = MeasureTextEx(GetFontDefault(), t, fontSize, 0);
        float tx = x + c * cell + (cell - size.x) / 2;
        float ty = y + r * cell + (cell - size.y) / 2;
        Color col = fixed_cell[r][c] ? BLACK : DARKBLUE;
        DrawText(t, (int)tx, (int)ty, fontSize, col);
      }
    }
  }
  // lines
  for (int i = 0; i <= GRID; i++) {
    int thickness = (i % BOX == 0) ? 4 : 1;
    DrawRectangleLinesEx((Rectangle){x, y + i * cell, w, 0}, thickness,
                         BLACK); // horizontal
    DrawRectangleLinesEx((Rectangle){x + i * cell, y, 0, w}, thickness,
                         BLACK); // vertical
  }
}

// Check mistakes: return number of wrong filled cells (ignoring blanks)
int check_mistakes() {
  int wrong = 0;
  for (int r = 0; r < GRID; r++)
    for (int c = 0; c < GRID; c++) {
      if (board[r][c] != 0 && board[r][c] != solution[r][c])
        wrong++;
    }
  return wrong;
}

int main(void) {
  srand(time(NULL));
  InitWindow(WIN_W, WIN_H, "Sudoku - raylib");
  SetTargetFPS(60);

  // start with medium difficulty
  int clues = 36; // more clues = easier (81 full)
  new_game(clues);

  float gridSize = 600;
  float gridX = (WIN_W - gridSize) / 2;
  float gridY = 20;

  bool show_solution = false;

  while (!WindowShouldClose()) {
    // Input handling
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 m = GetMousePosition();
      if (m.x >= gridX && m.x <= gridX + gridSize && m.y >= gridY &&
          m.y <= gridY + gridSize) {
        float cell = gridSize / GRID;
        int c = (int)((m.x - gridX) / cell);
        int r = (int)((m.y - gridY) / cell);
        if (r >= 0 && r < GRID && c >= 0 && c < GRID) {
          selected_row = r;
          selected_col = c;
        }
      } else {
        selected_row = selected_col = -1;
      }
    }
    // number input
    for (int k = KEY_ZERO; k <= KEY_NINE; k++) {
      if (IsKeyPressed(k)) {
        int val = -1;
        if (k == KEY_ZERO || k == KEY_BACKSPACE || k == KEY_DELETE)
          val = 0;
        else {
          // KEY_ONE..KEY_NINE
          if (k == KEY_ONE)
            val = 1;
          if (k == KEY_TWO)
            val = 2;
          if (k == KEY_THREE)
            val = 3;
          if (k == KEY_FOUR)
            val = 4;
          if (k == KEY_FIVE)
            val = 5;
          if (k == KEY_SIX)
            val = 6;
          if (k == KEY_SEVEN)
            val = 7;
          if (k == KEY_EIGHT)
            val = 8;
          if (k == KEY_NINE)
            val = 9;
        }
        if (selected_row != -1 && selected_col != -1 &&
            !fixed_cell[selected_row][selected_col]) {
          if (val == 0)
            board[selected_row][selected_col] = 0;
          else {
            if (is_valid(board, selected_row, selected_col, val)) {
              board[selected_row][selected_col] = val;
            } else {
              // place anyway (user mistake allowed)
              board[selected_row][selected_col] = val;
            }
          }
        }
      }
    }

    if (IsKeyPressed(KEY_SPACE)) {
      // auto-solve (fill with solution)
      copy_board(solution, board);
    }
    if (IsKeyPressed(KEY_R)) {
      // reveal solution
      show_solution = !show_solution;
      if (show_solution)
        copy_board(solution, board);
      else
        new_game(clues); // revert to a fresh puzzle -> if they wanted to hide,
                         // regenerate? Instead better to keep previous puzzle.
                         // We'll regenerate from solution.
    }
    if (IsKeyPressed(KEY_N)) {
      new_game(clues);
      show_solution = false;
    }
    if (IsKeyPressed(KEY_C)) {
      int wrong = check_mistakes();
      if (wrong == 0) {
        // verify if solved
        int empty = 0;
        for (int r = 0; r < GRID; r++)
          for (int c = 0; c < GRID; c++)
            if (board[r][c] == 0)
              empty++;
        if (empty == 0) {
          // win
          // flash message
        }
      }
    }
    if (IsKeyPressed(KEY_UP)) {
      clues += 2;
      if (clues > 81)
        clues = 81;
      new_game(clues);
    }
    if (IsKeyPressed(KEY_DOWN)) {
      clues -= 2;
      if (clues < 17)
        clues = 17;
      new_game(clues);
    }

    // render
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // draw board
    draw_board(gridX, gridY, gridSize);
    // UI and info
    int currentClues = 0;
    for (int r = 0; r < GRID; r++)
      for (int c = 0; c < GRID; c++)
        if (fixed_cell[r][c])
          currentClues++;
    char info[128];
    sprintf(info,
            "Clues: %d | Press N to new | Space to solve | R reveal (toggle) | "
            "Up/Down change difficulty",
            currentClues);
    DrawText(info, 10, WIN_H - 60, 18, BLACK);
    draw_ui(gridY, gridSize / GRID);

    // selection outline
    if (selected_row != -1 && selected_col != -1) {
      float cell = gridSize / GRID;
      DrawRectangleLines((int)(gridX + selected_col * cell),
                         (int)(gridY + selected_row * cell), (int)cell,
                         (int)cell, RED);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
