
#include "power.h"

void power_hal_wait(){
	
	EXTWAKE = 7; // EXTINT0 will awake the processor
	PCON |= 0x01;
	
}

void power_hal_deep_sleep(){
	PLLCON = 0x01;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	EXTWAKE = 7; // EXTINT0 will awake the processor
	PCON |= 0x02;
	Switch_to_PLL();
}

