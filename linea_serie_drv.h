#ifndef LINEA_SERIE_DRV_H
#define LINEA_SERIE_DRV_H

#include "linea_serie_hal.h"

//#include "gpio_hal.h"
//#include "SWI.h"



void linea_serie_drv_inicializar(void (*callback_fifo_encolar_param)(), uint8_t id_ev_RX_SERIE, uint8_t id_ev_TX_SERIE, void (*callback_gpio_hal_sentido_param)(), void (*callback_gpio_hal_escribir_param)(), uint32_t (*callback_gpio_hal_leer_param)(),int GPIO_SERIE_ERROR_PARAM, int GPIO_SERIE_ERROR_BITS_PARAM, int GPIO_HAL_PIN_DIR_OUTPUT_PARAM);

void linea_serie_drv_leer(uint8_t caracter);

void linea_serie_drv_enviar_array(uint8_t cadena[]);

void linea_serie_drv_continuar_envio(void);

void linea_serie_drv_escribir(uint8_t caracter);

#endif

