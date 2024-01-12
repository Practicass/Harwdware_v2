
#include "int_externas_hal.h"


static void (*callback_fifo_encolar_eint1)(); 		//puntero a la funcion que encola el evento para la interrupcion externa 1
static void (*callback_fifo_encolar_eint2)();		//puntero a la funcion que encola el evento	para la interrupcion externa 2
static uint8_t BOTON1;								
static uint8_t BOTON2;								


//rutina de interrupción que encola el evento BOTON en la cola fifo, 
//desactiva las interrupciones eint1 y limpia la interrupción
void eint1_ISR (void) __irq {
    callback_fifo_encolar_eint1(BOTON1, 1);        
    VICIntEnable = VICIntEnable & 0xfff7fff;
    VICIntEnClr = 0x00008000;  
	VICVectAddr = 0;
		
}
//rutina de interrupción que encola el evento BOTON en la cola fifo, 
//desactiva las interrupciones eint2 y limpia la interrupción
void eint2_ISR (void) __irq {

    callback_fifo_encolar_eint2(BOTON2, 2);        
    VICIntEnable = VICIntEnable & 0xfffeffff;
	VICIntEnClr = 0x00010000;
	VICVectAddr = 0;
}


//función que define las estructuras de datos necesarias para su correcto funcionamiento y 
// configura las interrupciones eint1
void eint1_init ( void (*callback_fifo_encolar_eint1_param)(), uint8_t BOTON_PARAM) {

	VICVectAddr2 = (unsigned long)eint1_ISR;							// Establece la dirección de la rutina de interrupción para eint1 en el vector de interrupciones

	PINSEL0 = PINSEL0 & 0xcfffffff;										//Configura la función del pin P0.14 como eint1
	PINSEL0 = PINSEL0 | 0x20000000;						
	VICVectCntl2 = 0x20 | 15;                   						//Configura la interrupcion, 0x20 habilita las interrupciones vectorizadas y 15 es el numero de interrupción eint1
    VICIntEnable = VICIntEnable | 0x00008000;                  			//Habilita la interrupción eint1
	callback_fifo_encolar_eint1 = callback_fifo_encolar_eint1_param;
	BOTON1 = BOTON_PARAM;
}

//función que define las estructuras de datos necesarias para su correcto funcionamiento y 
// configura las interrupciones eint2
void eint2_init (void (*callback_fifo_encolar_eint2_param)(), uint8_t BOTON_PARAM) {

	VICVectAddr3 = (unsigned long)eint2_ISR;							//Establece la dirección de la rutina de interrupción para eint2 en el vector de interrupción

	PINSEL0 		= PINSEL0 & 0x3fffffff;								//Configura la función del pin P0.15 como eint2
	PINSEL0 		= PINSEL0 | 0x80000000;	
	VICVectCntl3 = 0x20 | 16;               							//Configura la interrupcion, 0x20 habilita las interrupciones vectorizadas y 16 es el numero de interrupción eint2
  	VICIntEnable = VICIntEnable | 0x00010000; 							//Habilita la interrupción eint2
  	callback_fifo_encolar_eint2 = callback_fifo_encolar_eint2_param;
	BOTON2 = BOTON_PARAM;
}

//Habilita la interrupcion eint1
void eint1_enable(void){
	VICIntEnable = VICIntEnable | 0x00008000;  		
}
//Habilita la interrupcion eint2
void eint2_enable(void){
	VICIntEnable = VICIntEnable | 0x00010000;  		
}

//Comprueba si la interrupción eint1 sigue activa
unsigned int eint1_hold(void){
	EXTINT = EXTINT | ~2;			//Se guarda la flag de interrupción de eint1, estará activa si en el bit 1 hay un 1
    if ((EXTINT & 2) == 2) {		
        return 1;
    }
    else return 0;
}

//Comprueba si la interrupción eint2 sigue activa
unsigned int eint2_hold(void){		//Se guarda la flag de interrupción de eint1, estará activa si en el bit 2 hay un 1
	EXTINT = EXTINT | ~4;
    if ((EXTINT & 4) == 4) {
        return 1;
    }
    else return 0;
}




