
#include <LPC210X.H>  
#include "temporizador_hal.h"
  


static unsigned int timer1_int_count = 0;
static uint32_t periodoAux = 0;
static void (*funcion_callback2)();

//rutina de tratamiento de la interrupción TIMER1 que incrementa el contador de interrupciones, 
//ejecuta la función callback y limpia la interupción generada
void timer1_ISR (void) __irq {
	
		timer1_int_count++;
    funcion_callback2();
		T1IR = 1;		// Clear interrupt flag	
    VICVectAddr = 0;                            // Acknowledge Interrupt
}





//funcion que programa un contador para que pueda ser utilizado
void temporizador_hal_iniciar(){
		timer1_int_count = 0;                   
		//T1PR = 1510;
		T1PR = 14999;								//Interrumpe cada 1ms
		T1MCR = 3;									// Genera una interrumpción y reinicia la cuenta cuando se alcance el valor de MR0when the value of MR0 is reached
		T1TCR = 0;
		T1MR0 = 0;					
		VICVectAddr5 = (unsigned long)timer1_ISR;	//Establece la dirección de la rutina de interrupción para TIMER1 en el vector de interrupciones
		VICVectCntl5 = 0x20 | 5;					//Configura la interrupcion, 0x20 habilita las interrupciones vectorizadas y 5 es el numero de interrupción TIMER1




}

//funcion que lee el tiempo que lleva contando el contador desde 
//la ultima vez que se ejecutó temporizador_hal_empezar y lo devuelve 
//en ticks
uint64_t temporizador_hal_leer(){
    return timer1_int_count *periodoAux*14999 + T1PC;	
}

//Se detiene el Timer Counter y Prescale Counter y devuelve el tiempo en ticks transcurrido desde el ultimo temporizador_hal_empezar
uint64_t temporizador_hal_parar(){
	T1TCR = 3;//poner aqui 2 igual?	
    T1TCR = 0;
    return temporizador_hal_leer();
}

//Habilita las interrupciones TIMER1 y se reinicia Timer Counter y Prescale Counter
void temporizador_hal_empezar(){
			T1TCR = 1;
    VICIntEnable = VICIntEnable | 0x20;			

}

//funcion dependiente del hardware (timer1)
//que programa el reloj para que llame a la funci?n de callback cada
//periodo. El periodo se indica en ms. Si el periodo es cero se para el
//temporizador
void temporizador_hal_reloj (uint32_t periodo, void
(*funcion_callback)()){
	funcion_callback2 = funcion_callback;
	periodoAux = periodo;
	T1MR0 = periodo;

}


