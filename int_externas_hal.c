
#include "int_externas_hal.h"
static void (*callback_fifo_encolar_eint1)();
static void (*callback_fifo_encolar_eint2)();
static uint8_t BOTON1;
static uint8_t BOTON2;


//// variable para comprobar que se hacen las interrupciones que deber?an hacerse
//static volatile unsigned int eint1_count = 0;
//// variable que se activa al detectar una nueva pulsaci?n
//static volatile int eint1_nueva_pulsacion = 0;
//// variable para comprobar que se hacen las interrupciones que deber?an hacerse
//static volatile unsigned int eint2_count = 0;
//// variable que se activa al detectar una nueva pulsaci?n
//static volatile int eint2_nueva_pulsacion = 0;

void eint1_ISR (void) __irq {
	//eint1_count++;
    callback_fifo_encolar_eint1(BOTON1, 1);        
	//eint1_nueva_pulsacion = 1;
    VICIntEnable = VICIntEnable & 0xfff7fff;
    VICIntEnClr = 0x00008000;  
	VICVectAddr = 0;
		
}

void eint2_ISR (void) __irq {
	//eint2_count++;
    callback_fifo_encolar_eint2(BOTON2, 2);        
	//eint2_nueva_pulsacion = 1;
    VICIntEnable = VICIntEnable & 0xfffeffff;
	  VICIntEnClr = 0x00010000;
		VICVectAddr = 0;
}



void eint1_init ( void (*callback_fifo_encolar_eint1_param)(), uint8_t BOTON_PARAM) {
// NOTA: seg?n el manual se puede configurar c?mo se activan las interrupciones: por flanco o nivel, alta o baja. 
// Se usar?an los registros EXTMODE y EXTPOLAR. 
// Sin embargo parece que el simulador los ignora y no aparecen en la ventana de ocnfiguraci?n de EXT Interrupts
// configure EXTINT0 active if a rising-edge is detected
//	EXTMODE 	=	1; //1 edge, 0 level
//	EXTPOLAR	=	1; // 1 high, rising-edge; 0 low, falling-edge
//  prueba = EXTMODE;
	//eint1_nueva_pulsacion = 0;
	//eint1_count = 0;   	
	// configuration of the IRQ slot number 2 of the VIC for EXTINT0
	VICVectAddr2 = (unsigned long)eint1_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 15 is the number of the interrupt assigned. Number 15 is the EINT0 (see table 40 of the LPC2105 user manual  
	PINSEL0 		= PINSEL0 & 0xcfffffff;	//Sets bits 0 and 1 to 0
	PINSEL0 		= PINSEL0 | 0x20000000;					//Enable the EXTINT0 interrupt
	VICVectCntl2 = 0x20 | 15;                   
    VICIntEnable = VICIntEnable | 0x00008000;                  // Enable EXTINT0 Interrupt
	callback_fifo_encolar_eint1 = callback_fifo_encolar_eint1_param;
	BOTON1 = BOTON_PARAM;
}

void eint2_init (void (*callback_fifo_encolar_eint2_param)(), uint8_t BOTON_PARAM) {
// NOTA: seg?n el manual se puede configurar c?mo se activan las interrupciones: por flanco o nivel, alta o baja. 
// Se usar?an los registros EXTMODE y EXTPOLAR. 
// Sin embargo parece que el simulador los ignora y no aparecen en la ventana de ocnfiguraci?n de EXT Interrupts
// configure EXTINT0 active if a rising-edge is detected
//	EXTMODE 	=	1; //1 edge, 0 level
//	EXTPOLAR	=	1; // 1 high, rising-edge; 0 low, falling-edge
//  prueba = EXTMODE;
	//eint2_nueva_pulsacion = 0;
	//eint2_count = 0;  	
	// configuration of the IRQ slot number 2 of the VIC for EXTINT0
	VICVectAddr3 = (unsigned long)eint2_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 16 is the number of the interrupt assigned. Number 16 is the EINT0 (see table 40 of the LPC2105 user manual  
	PINSEL0 		= PINSEL0 & 0x3fffffff;	//Sets bits 0 and 1 to 0
	PINSEL0 		= PINSEL0 | 0x80000000;					//Enable the EXTINT0 interrupt
	VICVectCntl3 = 0x20 | 16;                   
  VICIntEnable = VICIntEnable | 0x00010000;                  // Enable EXTINT0 Interrupt
  	callback_fifo_encolar_eint2 = callback_fifo_encolar_eint2_param;
		BOTON2 = BOTON_PARAM;
}


void eint1_enable(void){
	VICIntEnable = VICIntEnable | 0x00008000;  
}
void eint2_enable(void){
	VICIntEnable = VICIntEnable | 0x00010000;  
}

unsigned int eint1_hold(void){
	EXTINT = EXTINT | ~2;
    if ((EXTINT & 2) == 2) {
        return 1;
    }
    else return 0;
}

unsigned int eint2_hold(void){
	EXTINT = EXTINT | ~4;
    if ((EXTINT & 4) == 4) {
        return 1;
    }
    else return 0;
}




