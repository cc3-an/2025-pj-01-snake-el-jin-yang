#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Tarea 1 */
game_state_t* create_default_state() {

  unsigned int num_rows = 18;  
  unsigned int num_cols = 20; 

  game_state_t* state = malloc(sizeof(game_state_t)); 
  state->num_rows = num_rows;
  state->board = malloc(num_rows * sizeof(char*)); 

  for (unsigned int i = 0; i < num_rows; i++) {
    state->board[i] = malloc((num_cols + 1) * sizeof(char)); 
    for (unsigned int j = 0; j < num_cols; j++) {
      if (i == 0 || i == num_rows - 1 || j == 0 || j == num_cols - 1) {
        state->board[i][j] = '#';
      } else {
        state->board[i][j] = ' ';  
      }
    }
    
    state->board[i][num_cols] = '\0';  
    
  }

  state->board[2][9] = '*';
  state->board[2][2] = 'd';  
  state->board[2][3] = '>';  
  state->board[2][4] = 'D'; 
  state->num_snakes = 1;
  
  state->snakes = malloc(sizeof(snake_t));
  
  state->snakes[0].tail_row = 2;
  state->snakes[0].tail_col = 2;
  state->snakes[0].head_row = 2;
  state->snakes[0].head_col = 4;
  state->snakes[0].live = true;

  return state;
}







/* Tarea 2 */

void free_state(game_state_t* state) {
  for (unsigned int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);  
  }
  
  free(state->board);
  free(state->snakes);
  free(state);
  
}








/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  for (unsigned int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }
}

void save_board(game_state_t* state, char* filename) {
  FILE* file = fopen(filename, "w");
  print_board(state, file);
  fclose(file);
}






/* Tarea 4.1 */
static bool is_tail(char c) {
  return (c == 'w' || c == 'a' || c == 's' || c == 'd');
}

static bool is_head(char c) {
  return (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x');
}

static bool is_snake(char c) {
  return (c == 'w' || c == 'a' || c == 's' || c == 'd' || 
          c == '^' || c == '<' || c == 'v' || c == '>' || 
          c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x');
}

static char body_to_tail(char c) {
  if (c == '^') return 'w';
  if (c == '<') return 'a';
  if (c == 'v') return 's';
  if (c == '>') return 'd';
  return '?'; 
}

static char head_to_body(char c) {
  if (c == 'W') return '^';
  if (c == 'A') return '<';
  if (c == 'S') return 'v';
  if (c == 'D') return '>';
  return '?'; 
}

static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row; 
}

static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;  
}

char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
    return state->board[row][col]; 
}

void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
    state->board[row][col] = ch; 
}

#define BOARD_ROWS 10 
#define BOARD_COLS 20 







/* Tarea 4.2 */
static char next_square(game_state_t* state, unsigned int snum) {

    unsigned int head_row = state->snakes[snum].head_row;
    unsigned int head_col = state->snakes[snum].head_col;
    char direction = get_board_at(state, head_row, head_col);
    unsigned int new_row = get_next_row(head_row, direction);
    unsigned int new_col = get_next_col(head_col, direction);

    return get_board_at(state, new_row, new_col);
}







/* Tarea 4.3 */
static void update_head(game_state_t* state, unsigned int snum) {

    snake_t* snake = &state->snakes[snum];
    unsigned int old_row = snake->head_row;
    unsigned int old_col = snake->head_col;
    char head_char = get_board_at(state, old_row, old_col);
    unsigned int new_row = get_next_row(old_row, head_char);
    unsigned int new_col = get_next_col(old_col, head_char);

    set_board_at(state, old_row, old_col, head_to_body(head_char));
    set_board_at(state, new_row, new_col, head_char);

    snake->head_row = new_row;
    snake->head_col = new_col;
}







/* Tarea 4.4 */
static void update_tail(game_state_t* state, unsigned int snum){
    snake_t* snake = &state->snakes[snum];
    unsigned int tail_row = snake->tail_row;
    unsigned int tail_col = snake->tail_col;
    char tail_char = get_board_at(state, tail_row, tail_col);

    set_board_at(state, tail_row, tail_col, ' ');

    unsigned int next_row = get_next_row(tail_row, tail_char);
    unsigned int next_col = get_next_col(tail_col, tail_char);
    char next_char = get_board_at(state, next_row, next_col);
    char new_tail = body_to_tail(next_char);

    set_board_at(state, next_row, next_col, new_tail);

    snake->tail_row = next_row;
    snake->tail_col = next_col;
}








/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
    for (unsigned int i = 0; i < state->num_snakes; i++) {
        snake_t* snake = &state->snakes[i];
        if (!snake->live) continue;
        
        unsigned int head_row = snake->head_row;
        unsigned int head_col = snake->head_col;
        char head_char = get_board_at(state, head_row, head_col);
        unsigned int new_row = get_next_row(head_row, head_char);
        unsigned int new_col = get_next_col(head_col, head_char);
        char next_char = get_board_at(state, new_row, new_col);

        if (next_char == '#' || is_snake(next_char)) {
            set_board_at(state, head_row, head_col, 'x');
            snake->live = false;
            continue;
        }

        bool grow = false;
        if (next_char == '*') {
            grow = true;
            if (add_food != NULL) {
                add_food(state);
            }
        }

        set_board_at(state, head_row, head_col, head_to_body(head_char));
        set_board_at(state, new_row, new_col, head_char);
        
        snake->head_row = new_row;
        snake->head_col = new_col;

        if (!grow) {
            update_tail(state, i);
        }
    }
}








/* Tarea 5 */
game_state_t* load_board(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) return NULL;

    char** lines = NULL;
    unsigned int capacity = 8; 
    unsigned int num_rows = 0;
    lines = malloc(capacity * sizeof(char*));

    char buffer[1024]; 
    while (fgets(buffer, sizeof(buffer), file)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        if (num_rows >= capacity) {
            capacity *= 2;
            lines = realloc(lines, capacity * sizeof(char*));
        }
        lines[num_rows] = malloc((len + 1) * sizeof(char));
        strcpy(lines[num_rows], buffer);
        num_rows++;
    }

    fclose(file);

    game_state_t* state = malloc(sizeof(game_state_t));
    state->num_rows = num_rows;
    state->board = lines;
    state->num_snakes = 0;
    state->snakes = NULL;

    return state;
}









/* Tarea 6.1 */
void find_head(game_state_t* state, unsigned int snum) {
    snake_t* snake = &state->snakes[snum];
    unsigned int row = snake->tail_row;
    unsigned int col = snake->tail_col;
    char ch;

    while (true) {
        ch = get_board_at(state, row, col);
        if (is_head(ch)) break;
        unsigned int next_row = get_next_row(row, ch);
        unsigned int next_col = get_next_col(col, ch);
        row = next_row;
        col = next_col;
    }

    snake->head_row = row;
    snake->head_col = col;
}









/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
    unsigned int snake_count = 0;
    for (unsigned int row = 0; row < state->num_rows; row++) {
        for (unsigned int col = 0; state->board[row][col] != '\0'; col++) {
            if (is_tail(state->board[row][col])) {
                snake_count++;
            }
        }
    }

    state->num_snakes = snake_count;
    state->snakes = malloc(sizeof(snake_t) * snake_count);

    unsigned int snum = 0;
    for (unsigned int row = 0; row < state->num_rows; row++) {
        for (unsigned int col = 0; state->board[row][col] != '\0'; col++) {
            if (is_tail(state->board[row][col])) {
                state->snakes[snum].tail_row = row;
                state->snakes[snum].tail_col = col;
                state->snakes[snum].live = true;
                find_head(state, snum);
                snum++;
            }
        }
    }

    return state;
}
