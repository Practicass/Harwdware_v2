

#ifndef TEMPORIZADOR_DRV_H
#define TEMPORIZADOR_DRV_H


#include "temporizador_hal.h"

#include "eventos.h"




uint32_t __swi(0) clock_get_us(void);

//funci?n que programa un contador para que pueda ser utilizado.
void temporizador_drv_iniciar(void);

//funci?n que inicia la cuenta de un contador de forma indefinida
void temporizador_drv_empezar(void);

//funci?n que lee el tiempo que lleva contando el contador desde la 
//?ltima vez que se ejecut? temporizador_drv_empezar y lo devuelve 
//en microsegundos.
uint64_t temporizador_drv_leer(void);

//detiene el contador y devuelve el
//tiempo transcurrido desde el ?ltimo temporizador_drv_empezar
uint64_t temporizador_drv_parar(void); 


//funci?n que programa el reloj para que encole un evento peri?dicamente 
//en la cola del planificador. El periodo se indica en ms
void temporizador_drv_reloj(uint32_t periodo, void
(*funcion_encolar_evento)(), EVENTO_T ID_evento);

#endif
