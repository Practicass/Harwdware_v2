
#ifndef HELLO_WORLD_H
#define HELLO_WORLD_H

#include <inttypes.h>

#include "gpio_hal.h"
#include "temporizador_drv.h"
#include <math.h>
#include "alarmas.h"



typedef void* funciontype;


//actualiza el contador 
void hello_world_tick_tack(void);

//configura los pines correspondientes y activa la alarma para que encole el evento cada periodo ms
void hello_world_inicializar(GPIO_HAL_PIN_T gpio_inicial, uint8_t num_bits,  enum gpio_hal_pin_dir_t direccion,  void (*callback_fifo_encolar_param)(), int GPIO_HELLO_WORLD_BITS_PARAM);

//aumenta el contador y encola el evento para visualizar el valor
void hello_world_tratar_evento(void);


#endif
