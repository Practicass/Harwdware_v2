
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
		hello_world_inicializar(GPIO_HELLO_WORLD, GPIO_HELLO_WORLD_BITS, GPIO_HAL_PIN_DIR_OUTPUT,FIFO_encolar, GPIO_HELLO_WORLD_BITS);
		visualizar_inicializar(GPIO_HELLO_WORLD, GPIO_HELLO_WORLD_BITS);
		//
		linea_serie_drv_inicializar(FIFO_encolar, ev_RX_SERIE,ev_TX_SERIE, gpio_hal_sentido, gpio_hal_escribir,gpio_hal_leer, GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS, GPIO_HAL_PIN_DIR_OUTPUT);
		botones_init(FIFO_encolar,alarma_activar);
		juego_inicializar(gpio_hal_sentido, gpio_hal_escribir,gpio_hal_leer, GPIO_JUEGO_ERROR, GPIO_JUEGO_ERROR_BITS, GPIO_HAL_PIN_DIR_OUTPUT, FIFO_estadisticas, FIFO_reiniciar_estadisticas);
		//
		WD_hal_inicializar(1);
		WD_hal_feed();
		alarma_activar(DEEP_SLEEP, USUARIO_AUSENTE, 0);
	
    while(idEvento != ALARMA_OVERFLOW){
			//uint8_t hayEvento = 0; //descomentar comprobar overflow cola
    	uint8_t hayEvento =  FIFO_extraer(&idEvento, &auxData); //comentar para comprobar overflow cola
			
    	if (hayEvento != 0) {
				WD_hal_feed();
				if(idEvento == ALARMA_OVERFLOW){
					gpio_hal_escribir( GPIO_OVERFLOW, GPIO_OVERFLOW_BITS,  GPIO_HAL_PIN_DIR_OUTPUT);
				}else if(idEvento == TIMER){
					alarma_tratar_evento();
				}else if(idEvento == ev_RX_SERIE){
					juego_tratar_evento(idEvento,auxData);
				}else if(idEvento  == ev_TX_SERIE){
					uint32_t t2 = clock_get_us();
					juego_tratar_evento(idEvento, t2);
					alarma_activar(DEEP_SLEEP, USUARIO_AUSENTE, 0);
				}else if(idEvento == ev_VISUALIZAR_HELLO){
					visualizarHello(auxData);
				}else if(idEvento == ev_LATIDO){
					hello_world_tratar_evento();
				}else if(idEvento == BOTON){
					botones_pulsar(auxData);
					alarma_activar(DEEP_SLEEP, USUARIO_AUSENTE, 0);
					juego_tratar_evento(idEvento, auxData);
				}else if(idEvento == BOTON_EINT1_ALARM || idEvento == BOTON_EINT2_ALARM){
					botones_monitorizar(idEvento);
				}else if(idEvento == ev_JUEGO){
					juego_tratar_evento(idEvento, auxData);
				}else if(idEvento == DEEP_SLEEP){
					power_hal_deep_sleep();
					alarma_activar(DEEP_SLEEP, USUARIO_AUSENTE, 0);// PUEDE QUE SEA PERIODICA
				}
		}else{
			power_hal_wait();
					
		}
       
    }
}



