
#ifndef TEMPORIZADOR_HAL_H
#define TEMPORIZADOR_HAL_H

#define TEMPORIZADOR_HAL_TICKS2US  15

#include <inttypes.h>




//funci?n que programa un contador para que pueda ser utilizado
void temporizador_hal_iniciar(void);

//funci?n que inicia la cuenta de un contador de forma indefinida
void temporizador_hal_empezar(void);

//funci?n que lee el tiempo que lleva contando el contador desde 
//la ?ltima vez que se ejecut? temporizador_hal_empezar y lo devuelve 
//en ticks
uint64_t temporizador_hal_leer(void);

//detiene el contador y devuelve el tiempo en ticks transcurrido desde 
//el ?ltimo temporizador_hal_empezar uint64_t temporizador_hal_parar(void);
uint64_t temporizador_hal_parar(void);

//funci?n dependiente del hardware (timer1)
//que programa el reloj para que llame a la funci?n de callback cada
//periodo. El periodo se indica en ms. Si el periodo es cero se para el
//temporizador
void temporizador_hal_reloj (uint32_t periodo, void
(*funcion_callback)());





#endif

