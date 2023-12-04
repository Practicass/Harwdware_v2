

//#include "fifo.h"
#include "temporizador_drv.h"
#include <inttypes.h>
#include "conecta_K_2023.h"
#include "linea_serie_drv.h"


void juego_inicializar(void (*funcion_callbackAlarmasParam)());

void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData);


void conecta_K_visualizar_tablero_juego(void);

void conecta_K_visualizar_tiempo(uint32_t num);
