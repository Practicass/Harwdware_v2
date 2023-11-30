
#include "planificador.h"





void planificador(){
    EVENTO_T idEvento;
		uint32_t auxData;
		
	
		// inicializar gpio
    gpio_hal_iniciar();
    ////Inicializar la cola 
		//
    FIFO_inicializar(GPIO_OVERFLOW,GPIO_OVERFLOW_BITS);
		alarma_inicializar(FIFO_encolar);
		//hello_world_inicializar(GPIO_HELLO_WORLD, GPIO_HELLO_WORLD_BITS, GPIO_HAL_PIN_DIR_OUTPUT,FIFO_encolar, GPIO_HELLO_WORLD_BITS);
		//visualizar_inicializar(GPIO_HELLO_WORLD, GPIO_HELLO_WORLD_BITS);
		//
		linea_serie_drv_inicializar(FIFO_encolar, ev_RX_SERIE,UART0_CARACTER,CONTINUAR_ENVIO,ev_TX_SERIE);
		////juego_inicializar(FIFO_encolar);
		//
		//alarma_activar(HELLOWORLD, 0x8000000a,0);
	
    while(idEvento != ALARMA_OVERFLOW){
			//uint8_t hayEvento = 0; //descomentar comprobar overflow cola
    	uint8_t hayEvento =  FIFO_extraer(&idEvento, &auxData); //comentar para comprobar overflow cola
			
    	if (hayEvento != 0) {
				if(idEvento == ALARMA_OVERFLOW){
					gpio_hal_escribir( GPIO_OVERFLOW, GPIO_OVERFLOW_BITS,  GPIO_HAL_PIN_DIR_OUTPUT);
				}else if(idEvento == TIMER){
					alarma_tratar_evento();
				}else if(idEvento == ev_RX_SERIE){
					linea_serie_drv_enviar_array(auxData);
				}else if(idEvento  == CONTINUAR_ENVIO){
					linea_serie_drv_continuar_envio();
				}else if(idEvento  == ev_TX_SERIE){
					//idEvento = idEvento;
				}else if(idEvento == ev_VISUALIZAR_HELLO){
					visualizarHello(auxData);
				}else if(idEvento == ev_VISUALIZAR_CUENTA){
					//visualizarCuenta(auxData);
				}else if(idEvento == ev_LATIDO){
					hello_world_tratar_evento();
				}if(idEvento == HELLOWORLD){
					hello_world_tick_tack();
				}
		}else{
			//power_hal_wait();
					
		}
       
    }
}



