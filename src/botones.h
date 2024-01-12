#ifndef BOTONES
#define BOTONES

#include "int_externas_hal.h"
#include "eventos.h"
#include "gpio_hal.h"
#include <inttypes.h>

// Estados de los botones
enum ESTADO{
    PULSADO,
    NO_PULSADO
};

// Inicializa los botones
void botones_init(void (*callback_fifo_encolar_param)(), void (*callback_alarma_activar_param)());

// Modifica el estado de los botones y programa la alarma de comprobacion
void botones_pulsar(uint32_t auxData);

//Comprueba si los botones estan pulsados
void botones_monitorizar(EVENTO_T ID_evento);




#endif
