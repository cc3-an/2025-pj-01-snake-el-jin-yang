#include <stdio.h>
#include <string.h>

#include "snake_utils.h"
#include "state.h"

int main(int argc, char* argv[]) {
  char* in_filename = NULL;
  char* out_filename = NULL;
  game_state_t* state = NULL;

  // Parsea los argumentos recibidos
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i < argc - 1) {
      in_filename = argv[i + 1];
      i++;
      continue;
    }
    if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
      out_filename = argv[i + 1];
      i++;
      continue;
    }
    fprintf(stderr, "Usage: %s [-i filename] [-o filename]\n", argv[0]);
    return 1;
  }

  // NO MODIFIQUEN NADA ARRIBA DE ESTA LINEA.

  /* Tarea 7 */

  // Leer el tablero de un archivo, o crear un tablero por defecto.
  if (in_filename != NULL) {
    // TODO: cargar el tablero de in_filename
    // TODO: Si el archivo no existe, retornar -1
    // TODO: Despues llamar a initialize_snakes en el estado creado
  } else {
    // TODO: Cargar el estado por defecto.
  }

  // TODO: Actualizar el estado. Utilizar la funcion deterministic_food
  // (esta ya ha sido creada en snakes_utils.h) para agregar comida al
  // tablero)

  // Write updated board to file or stdout
  // Escribir el tablero actualizado al archivo o stdout
  if (out_filename != NULL) {
    // TODO: Guardar el tablero en out_filename
  } else {
    // TODO: Imprimir el tablero a stdout
  }

  // TODO: Liberen el estado creado

  return 0;
}
