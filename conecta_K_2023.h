/* guarda para evitar inclusiones m�ltiples ("include guard") */
#ifndef CONECTA_K_H_2023
#define CONECTA_K_H_2023

#include <inttypes.h>
#include "celda.h"
#include "tablero.h"


/* *****************************************************************************
 * declaracion de funciones visibles en el exterior conecta_K
 */

// función principal del juego
void conecta_K_jugar(void);

// función test del juego
void conecta_K_test(void);

// devuelve la longitud de la línea más larga
uint8_t conecta_K_buscar_alineamiento_c(TABLERO *t, uint8_t fila,
	uint8_t columna, uint8_t color, int8_t delta_fila, int8_t
	delta_columna);

// devuelve true si encuentra una línea de longitud mayor o igual a 4
uint8_t conecta_K_hay_linea_c_c(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);

// devuelve true si encuentra una línea de longitud mayor o igual a 4
uint8_t conecta_K_hay_linea_c_arm(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);


void conecta_K_visualizar_tablero(TABLERO *t, uint8_t pantalla[8][8]);

void conecta_K_test_cargar_tablero(TABLERO *t);

#if 0
// funciones a implementar en ARM

// devuelve la longitud de la línea más larga en un determinado sentido
uint8_t conecta_K_buscar_alineamiento_arm(TABLERO *t, uint8_t
	fila, uint8_t columna, uint8_t color, int8_t delta_fila, int8_t
	delta_columna);

// devuelve true si encuentra una línea de longitud mayor o igual a K
uint8_t conecta_K_hay_linea_arm_c(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);

// devuelve true si encuentra una línea de longitud mayor o igual a K
uint8_t conecta_K_hay_linea_arm_arm(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);


#endif // 0

/* *****************************************************************************
 * declaración funciones internas conecta_K
 */

int conecta_K_verificar_K_en_linea(TABLERO *t, uint8_t fila, uint8_t columna, uint8_t color);

#endif /* CONECTA_K_H_2023 */
