#ifndef LINEA_SERIE_DRV_H
#define LINEA_SERIE_DRV_H

#include "linea_serie_hal.h"

//#include "gpio_hal.h"
//#include "SWI.h"



void linea_serie_drv_inicializar(void (*callback_fifo_encolar_param)(), uint8_t idEvento, uint8_t idEvento2, uint8_t idEvento3, uint8_t idEvento4);

void linea_serie_drv_leer(uint8_t caracter);

void linea_serie_drv_enviar_array(uint8_t cadena[]);

void linea_serie_drv_continuar_envio(void);

#endif

