#ifndef _SNK_SNAKE_UTILS_H
#define _SNK_SNAKE_UTILS_H

#include <stdint.h>
#include "state.h"

// Define las teclas presionadas en modo interactivo
#define KEY_MOVEUP 0x77
#define KEY_MOVERIGHT 0x64
#define KEY_MOVEDOWN 0x73
#define KEY_MOVELEFT 0x61
#define KEY_QUIT 0x71

// Una funcion random deterministica simple. Lean sobre LFSR para aprender mas.
// Ref: https://en.wikipedia.org/wiki/Linear-feedback_shift_register
unsigned int det_rand(unsigned int* state);

// Deterministicamente genera comida en el tablero.
int deterministic_food(game_state_t* state);

// Genera comida en la esquina superior izquierda del tablero.
int corner_food(game_state_t* state);

// Cambia la direccion de un snake (no jugador).
void redirect_snake(game_state_t* state, char input_direction);

// De manera aleatoria causa que la snake elegida de un giro a la derecha o izquierda.
void random_turn(game_state_t* state, int snum);

#endif
