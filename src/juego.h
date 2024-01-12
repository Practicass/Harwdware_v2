

//#include "fifo.h"
#include "temporizador_drv.h"
#include <inttypes.h>
#include "conecta_K_2023.h"
#include "linea_serie_drv.h"
#include "alarmas.h"


//comienzo del juego, inicializa el tablero y muestra mensaje inicial por pantalla
void juego_inicializar(void (*callback_gpio_hal_sentido_param)(), void (*callback_gpio_hal_escribir_param)(), uint32_t (*callback_gpio_hal_leer_param)(), int GPIO_JUGAR_ERROR_PARAM, int GPIO_JUGAR_ERROR_BITS_PARAM, int GPIO_HAL_PIN_DIR_OUTPUT_PARAM, uint32_t (*callback_fifo_estadisticas_param)(), void (*callback_fifo_reiniciar_estadisticas_param)());

//maquina de estados de nuestro juego
void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData);


