

//#include "fifo.h"
#include "temporizador_drv.h"
#include <inttypes.h>
#include "conecta_K_2023.h"
#include "linea_serie_drv.h"
#include "alarmas.h"
#include "fifo.h"

void juego_inicializar(void (*callback_gpio_hal_sentido_param)(), void (*callback_gpio_hal_escribir_param)(), int GPIO_JUGAR_ERROR_PARAM, int GPIO_JUGAR_ERROR_BITS_PARAM, int GPIO_HAL_PIN_DIR_OUTPUT_PARAM);

void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData);


void conecta_K_visualizar_tablero_juego(void);
void tiempo_visualizar_tablero(uint32_t t2);

void conecta_K_visualizar_tiempo(uint32_t num);
void conecta_K_visualizar_movimiento_juego(void);
void mostrar_movimiento_cancelado(void);

