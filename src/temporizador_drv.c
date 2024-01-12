


#include "temporizador_drv.h"
static void (*callback_fifo_encolar)();
static uint8_t event;

//funci?n que programa un contador para que pueda ser utilizado.
void temporizador_drv_iniciar(void){
    temporizador_hal_iniciar();
}

//funci?n que inicia la cuenta de un contador de forma indefinida
void temporizador_drv_empezar(void){
    temporizador_hal_empezar();
}

//funci?n que lee el tiempo que lleva contando el contador desde la 
//?ltima vez que se ejecut? temporizador_drv_empezar y lo devuelve 
//en microsegundos.

uint32_t __SWI_0 (void) { return (uint32_t)temporizador_drv_leer();}

uint64_t temporizador_drv_leer(void){
	uint64_t aux = temporizador_hal_leer() / TEMPORIZADOR_HAL_TICKS2US;
    return aux;
}

//detiene el contador y devuelve el
//tiempo transcurrido desde el ?ltimo temporizador_drv_empezar
uint64_t temporizador_drv_parar(void){
    uint64_t aux = temporizador_hal_parar() /TEMPORIZADOR_HAL_TICKS2US;
    return aux;
}


void funcion_auxiliar(){
	callback_fifo_encolar(event);
}

//funci?n que programa el reloj para que encole un evento peri?dicamente 
//en la cola del planificador. El periodo se indica en ms
void temporizador_drv_reloj(uint32_t periodo, void
(*callback_fifo_encolar_param)(), EVENTO_T ID_evento){
		event = ID_evento;
		callback_fifo_encolar = callback_fifo_encolar_param;
    temporizador_hal_reloj(periodo,funcion_auxiliar);
		if(periodo == 0){
			temporizador_drv_parar();
		}else{
			temporizador_drv_iniciar();
			temporizador_drv_empezar();
		
		}	

}



