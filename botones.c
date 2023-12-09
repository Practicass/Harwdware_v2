


#include "botones.h"


enum ESTADO boton_1, boton_2 ; 						// Estado de los botones
void (*callback_alarma_activar)();					// Puntero a la funcion de activacion de la alarma de comprobacion


// Inicializa los botones
void botones_init(void (*callback_fifo_encolar_param)(), void (*callback_alarma_activar_param)()){
	boton_1 = NO_PULSADO;
	boton_2 = NO_PULSADO;
	eint1_init(callback_fifo_encolar_param, BOTON);
	eint2_init(callback_fifo_encolar_param, BOTON);
	callback_alarma_activar = callback_alarma_activar_param;
	
}

// Modifica el estado de los botones y programa la alarma de comprobacion
void botones_pulsar(uint32_t auxData){

	switch (auxData) {
		case 1:
			boton_1 = PULSADO;											//cambia el estado del boton
			callback_alarma_activar(BOTON_EINT1_ALARM,0x80000064,0); 	//programa la alarma de comprobacion
			break;
		case 2:
			boton_2 = PULSADO;											//cambia el estado del boton
			callback_alarma_activar(BOTON_EINT2_ALARM,0x80000064,0); 	//programa la alarma de comprobacion
			break;
	}
}

//Comprueba si los botones estan pulsados
void botones_monitorizar(EVENTO_T ID_evento){
	
	switch (ID_evento) {
		case BOTON_EINT1_ALARM:
			if(!eint1_hold()){
				callback_alarma_activar(BOTON_EINT1_ALARM, 0,0);  		//desactiva la alarma de comprobacion
				boton_1 = NO_PULSADO;									//cambia el estado del boton	
				eint1_enable();
			}
			
			break;
		case BOTON_EINT2_ALARM:
			if(!eint2_hold()){
				callback_alarma_activar(BOTON_EINT2_ALARM, 0,0);		//desactiva la alarma de comprobacion
				boton_1 = NO_PULSADO;									//cambia el estado del boton
				eint2_enable();
			}

			break;
	}
}




