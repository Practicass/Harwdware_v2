
#include "planificador.h"





void planificador(){
    EVENTO_T idEvento;
		uint32_t auxData;
		
	
		// inicializar gpio
    gpio_hal_iniciar();
    //Inicializar la cola 
	
    FIFO_inicializar(GPIO_OVERFLOW,GPIO_OVERFLOW_BITS);
		linea_serie_drv_inicializar(FIFO_encolar, ev_RX_SERIE,UART0_CARACTER,CONTINUAR_ENVIO,ev_TX_SERIE);
	


    while(idEvento != ALARMA_OVERFLOW){
			//uint8_t hayEvento = 0; //descomentar comprobar overflow cola
    	uint8_t hayEvento =  FIFO_extraer(&idEvento, &auxData); //comentar para comprobar overflow cola
			
    	if (hayEvento != 0) {
				if(idEvento == ALARMA_OVERFLOW){
					gpio_hal_escribir( GPIO_OVERFLOW, GPIO_OVERFLOW_BITS,  GPIO_HAL_PIN_DIR_OUTPUT);
				}else if(idEvento == ev_RX_SERIE){
					linea_serie_drv_enviar_array(auxData);
				}else if(idEvento  == CONTINUAR_ENVIO){
					linea_serie_drv_continuar_envio();
				}else if(idEvento  == ev_TX_SERIE){
					idEvento = idEvento;
				}
		}else{
			//power_hal_wait();
					
		}
       
    }
}
