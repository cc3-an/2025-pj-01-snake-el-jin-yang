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


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4.1 */
/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  return (c == 'w' || c == 'a' || c == 's' || c == 'd');
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  return (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x');
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return (c == 'w' || c == 'a' || c == 's' || c == 'd' || 
          c == '^' || c == '<' || c == 'v' || c == '>' || 
          c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x');
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^') return 'w';
  if (c == '<') return 'a';
  if (c == 'v') return 's';
  if (c == '>') return 'd';
  return '?'; 
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  if (c == 'W') return '^';
  if (c == 'A') return '<';
  if (c == 'S') return 'v';
  if (c == 'D') return '>';
  return '?';  
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;  
}


/**
 * Retorna cur_col + 1 si la variable c es '>', 'd' o 'D'.
 * Retorna cur_col - 1 si la variable c es '<', 'a' o 'A'.
 * Retorna cur_col de lo contrario
*/
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


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/

static char next_square(game_state_t* state, unsigned int snum) {
    unsigned int head_row = state->snakes[snum].head_row;
    unsigned int head_col = state->snakes[snum].head_col;

    char direction = get_board_at(state, head_row, head_col);

    unsigned int new_row = get_next_row(head_row, direction);
    unsigned int new_col = get_next_col(head_col, direction);

    return get_board_at(state, new_row, new_col);
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
    unsigned int head_row = state->snakes[snum].head_row;
    unsigned int head_col = state->snakes[snum].head_col;

    char direction = get_board_at(state, head_row, head_col);

    if (direction == '\0') {
        printf("Dirección inválida en (%d, %d)\n", head_row, head_col);
        return;
    }

    unsigned int new_row = get_next_row(head_row, direction);
    unsigned int new_col = get_next_col(head_col, direction);

    if (new_row >= state->board_height || new_col >= state->board_width) {
        printf("Nueva posición fuera de los límites del tablero: (%d, %d)\n", new_row, new_col);
        return;
    }

    state->snakes[snum].head_row = new_row;
    state->snakes[snum].head_col = new_col;

    char head_symbol = head_to_body(direction); 

    set_board_at(state, new_row, new_col, head_symbol);

}



/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_head(game_state_t* state, unsigned int snum) {
    // Acceder al objeto de la serpiente correspondiente
    snake_t* snake = &state->snakes[snum];

    // Obtener la posición de la cabeza de la serpiente (row, col)
    int head_row = snake->body[0].row;
    int head_col = snake->body[0].col;

    // Obtener la dirección en la que la serpiente se mueve (supongamos que es una estructura con dirección)
    direction_t dir = snake->direction;

    // Eliminar la cabeza actual en el tablero
    state->board[head_row][head_col] = ' '; // Se asume que el espacio vacío es representado por un espacio en blanco

    // Mover la cabeza según la dirección
    switch (dir) {
        case UP:
            head_row--;
            break;
        case DOWN:
            head_row++;
            break;
        case LEFT:
            head_col--;
            break;
        case RIGHT:
            head_col++;
            break;
        default:
            break;
    }

    // Actualizar la posición de la cabeza en la estructura del snake
    snake->body[0].row = head_row;
    snake->body[0].col = head_col;

    // Colocar la nueva cabeza en el tablero (usar un símbolo como 'H' para la cabeza)
    state->board[head_row][head_col] = 'H';  // Se asume que 'H' representa la cabeza de la serpiente
}



/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
    for (unsigned int i = 0; i < state->num_snakes; i++) {
        if (state->snakes[i].live) {
            update_head(state, i);  // Actualiza la cabeza de la serpiente
            update_tail(state, i);  // Actualiza la cola de la serpiente
        }
    }

    // Llamar a la función add_food para añadir comida
    add_food(state);
}


/* Tarea 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implementar esta funcion.
  return NULL;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  return;
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implementar esta funcion.
  return NULL;
}