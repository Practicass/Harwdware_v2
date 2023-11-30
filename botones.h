#ifndef BOTONES
#define BOTONES

#include "int_externas_hal.h"
#include "eventos.h"
#include "gpio_hal.h"
#include <inttypes.h>

enum ESTADO{
    PULSADO,
    NO_PULSADO
};


void botones_init(void (*callback_fifo_encolar_param)(), void (*callback_alarma_activar_param)());

void botones_pulsar(uint32_t auxData);

void botones_monitorizar(EVENTO_T ID_evento);




#endif
