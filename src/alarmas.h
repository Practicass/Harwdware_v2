#ifndef ALARMAS_H
#define ALARMAS_H


#include "temporizador_drv.h"
#include "gpio_hal.h"


//Función que inicializa las estructuras de datos necesarias para el funcionamiento de las alarmas 
//y activa un temporizador periodico cada 1ms
void alarma_inicializar(void (*callback_fifo_encolar_param)());

//Función que activa una alarma para un evento con un retardo determinado. Si el retardo es 0 se desactiva la alarma. 
//Si la alarma ya estaba activa se resetea.
void alarma_activar(EVENTO_T ID_evento, uint32_t retardo, uint32_t
auxData);

//Función que se llama cada vez que se produce una interrupción del temporizador. Se encarga de comprobar si alguna 
//alarma ha expirado y en ese caso encola el evento correspondiente en la fifo de eventos.
void alarma_tratar_evento(void);


#endif
