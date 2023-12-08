#ifndef ALARMAS_H
#define ALARMAS_H


#include "temporizador_drv.h"
#include "gpio_hal.h"



void alarma_inicializar(void (*callback_fifo_encolar_param)());

void alarma_activar(EVENTO_T ID_evento, uint32_t retardo, uint32_t
auxData);

void alarma_tratar_evento(void);


#endif
