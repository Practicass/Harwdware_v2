


#include "botones.h"

enum ESTADO boton_1, boton_2 ;
void (*callback_alarma_activar)();

void botones_init(void (*callback_fifo_encolar_param)(), void (*callback_alarma_activar_param)()){
	boton_1 = NO_PULSADO;
	boton_2 = NO_PULSADO;
	eint1_init(callback_fifo_encolar_param, BOTON);
	eint2_init(callback_fifo_encolar_param, BOTON);
	callback_alarma_activar = callback_alarma_activar_param;
	
}

void botones_pulsar(uint32_t auxData){

	switch (auxData) {
		case 1:
			boton_1 = PULSADO;
			callback_alarma_activar(BOTON_EINT1_ALARM,0x80000064,0);
			break;
		case 2:
			boton_2 = PULSADO;
			callback_alarma_activar(BOTON_EINT2_ALARM,0x80000064,0);
			break;
	}
}

void botones_monitorizar(EVENTO_T ID_evento){
	
	switch (ID_evento) {
		case BOTON_EINT1_ALARM:
			if(!eint1_hold()){
				callback_alarma_activar(BOTON_EINT1_ALARM, 0,0);
				boton_1 = NO_PULSADO;
				eint1_enable();
			}
			
			break;
		case BOTON_EINT2_ALARM:
			if(!eint2_hold()){
				callback_alarma_activar(BOTON_EINT2_ALARM, 0,0);
				boton_1 = NO_PULSADO;
				eint2_enable();
			}

			break;
	}
}




