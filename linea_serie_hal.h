
#ifndef LINEA_SERIE_HAL_H
#define LINEA_SERIE_HAL_H

#include <LPC210X.H>
#include <inttypes.h>




void linea_serie_ISR(void) __irq;

void linea_serie_hal_inicializar(void (*callback_linea_serie_drv_leer_param)(), void (*callback_linea_serie_drv_escribir_param)());

void linea_serie_hal_escribir(uint8_t caracter);

#endif

