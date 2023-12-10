

#ifndef TEMPORIZADOR_DRV_H
#define TEMPORIZADOR_DRV_H


#include "temporizador_hal.h"

#include "eventos.h"




uint32_t __swi(0) clock_get_us(void);

//funcion que programa un contador para que pueda ser utilizado.
void temporizador_drv_iniciar(void);

//funcion que inicia la cuenta de un contador de forma indefinida
void temporizador_drv_empezar(void);

//funcion que lee el tiempo que lleva contando el contador desde la 
//ultima vez que se ejecuto temporizador_drv_empezar y lo devuelve 
//en microsegundos.
uint64_t temporizador_drv_leer(void);

//detiene el contador y devuelve el
//tiempo transcurrido desde el ultimo temporizador_drv_empezar
uint64_t temporizador_drv_parar(void); 


//funcion que programa el reloj para que encole un evento periodicamente 
//en la cola del planificador. El periodo se indica en ms
void temporizador_drv_reloj(uint32_t periodo, void
(*funcion_encolar_evento)(), EVENTO_T ID_evento);

#endif
