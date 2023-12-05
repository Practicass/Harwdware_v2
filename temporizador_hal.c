
#include <LPC210X.H>  
#include "temporizador_hal.h"
  


static unsigned int timer1_int_count = 0;
static uint32_t periodoAux = 0;
static void (*funcion_callback2)();


//void timer0_ISR (void) __irq;    // Generate Interrupt 
//
//void timer_ISR (void) __irq {
//    timer0_int_count++;
//    T0IR = 1;                              // Clear interrupt flag
//    VICVectAddr = 0;                            // Acknowledge Interrupt
//}
void timer1_ISR (void) __irq {
	
		timer1_int_count++;
    funcion_callback2();
		T1IR = 1;		// Clear interrupt flag	
    VICVectAddr = 0;                            // Acknowledge Interrupt
}


//funci?n que programa un contador para que pueda ser utilizado
//void temporizador_hal_iniciar(){
//    timer0_int_count = 0;
//		
//	// configuration of Timer 0
//    T0PR = 2999;
//	T0MR0 = 10000;  // Interrumpe cada 0,05ms = 150.000-1 counts
//    T0MCR = 3;                     // Generates an interrupt and resets the count when the value of MR0 is reached
//    T0TCR = 1;                             // Timer0 Enable
//    // configuration of the IRQ slot number 0 of the VIC for Timer 0 Interrupt
//		VICVectAddr0 = (unsigned long)timer_ISR;          // set interrupt vector in 0
//    // 0x20 bit 5 enables vectored IRQs. 
//		// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual  
//		VICVectCntl0 = 0x20 | 4;  
//
//}
///* T  T0TCR = 1;  imer Counter 0 Interrupt executes each 10ms @ 60 MHz CPU Clock */
//
//
//
////funci?n que inicia la cuenta de un contador de forma indefinida
//void temporizador_hal_empezar(){
//    T0TCR = 1;
//    VICIntEnable = VICIntEnable | 0x10;
//    timer0_int_count = 0;
//    T0TC = 0;
//    T0PC = 0;
//}
//
////funci?n que lee el tiempo que lleva contando el contador desde 
////la ?ltima vez que se ejecut? temporizador_hal_empezar y lo devuelve 
////en ticks
//uint64_t temporizador_hal_leer(){
//    return timer0_int_count * 10000*TEMPORIZADOR_HAL_TICKS2US + T0TC*TEMPORIZADOR_HAL_TICKS2US + T0PC;
//}
//
//
////detiene el contador y devuelve el tiempo en ticks transcurrido desde 
////el ?ltimo temporizador_hal_empezar uint64_t temporizador_hal_parar(void);
//uint64_t temporizador_hal_parar(){
//    T0TCR = 0;
//    return temporizador_hal_leer();
//}





void temporizador_hal_iniciar(){
		timer1_int_count = 0;                   
		//T1PR = 1510;
		T1PR = 14999;
		T1MCR = 3;
		T1TCR = 0;
		VICVectAddr5 = (unsigned long)timer1_ISR;
		VICVectCntl5 = 0x20 | 5;
		T1MR0 = 0;



}

uint64_t temporizador_hal_leer(){
    return timer1_int_count *periodoAux*14999 + T1PC;
}

uint64_t temporizador_hal_parar(){
		T1TCR = 3;
    T1TCR = 0;
    return temporizador_hal_leer();
}

void temporizador_hal_empezar(){
			T1TCR = 1;
    VICIntEnable = VICIntEnable | 0x20;

}

//funci?n dependiente del hardware (timer1)
//que programa el reloj para que llame a la funci?n de callback cada
//periodo. El periodo se indica en ms. Si el periodo es cero se para el
//temporizador
void temporizador_hal_reloj (uint32_t periodo, void
(*funcion_callback)()){
	funcion_callback2 = funcion_callback;
	periodoAux = periodo;
	T1MR0 = periodo;

}


