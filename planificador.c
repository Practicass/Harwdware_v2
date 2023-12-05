
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
		linea_serie_drv_inicializar(FIFO_encolar, ev_RX_SERIE,UART0_CARACTER,CONTINUAR_ENVIO,ev_TX_SERIE);
		botones_init(FIFO_encolar,alarma_activar);
		juego_inicializar(FIFO_encolar);
		//
		WD_hal_inicializar(30);
		WD_hal_feed();
		//alarma_activar(DEEP_SLEEP, USUARIO_AUSENTE, 0);
	
    while(idEvento != ALARMA_OVERFLOW){
			//uint8_t hayEvento = 0; //descomentar comprobar overflow cola
    	uint8_t hayEvento =  FIFO_extraer(&idEvento, &auxData); //comentar para comprobar overflow cola
			
    	if (hayEvento != 0) {
				if(idEvento == ALARMA_OVERFLOW){
					gpio_hal_escribir( GPIO_OVERFLOW, GPIO_OVERFLOW_BITS,  GPIO_HAL_PIN_DIR_OUTPUT);
				}else if(idEvento == TIMER){
					alarma_tratar_evento();
				}else if(idEvento == ev_RX_SERIE){
					
					juego_tratar_evento(idEvento,auxData);
					//linea_serie_drv_enviar_array(bufferSalidaPlan);
				}else if(idEvento  == CONTINUAR_ENVIO){
					linea_serie_drv_continuar_envio();
				}else if(idEvento  == ev_TX_SERIE){
					//idEvento = idEvento;	
					uint32_t t2 = clock_get_us();
					juego_tratar_evento(idEvento, t2);
					WD_hal_feed();
				}else if(idEvento == ev_VISUALIZAR_HELLO){
					visualizarHello(auxData);
				}else if(idEvento == ev_VISUALIZAR_CUENTA){
					visualizarCuenta(auxData);
				}else if(idEvento == ev_LATIDO){
					hello_world_tratar_evento();
				}else if(idEvento == HELLOWORLD){
					hello_world_tick_tack();
				}else if(idEvento == BOTON){
					botones_pulsar(auxData);
					alarma_activar(DEEP_SLEEP, USUARIO_AUSENTE, 0);
					juego_tratar_evento(idEvento, auxData);
				}else if(idEvento == BOTON_EINT1_ALARM || idEvento == BOTON_EINT2_ALARM){
					botones_monitorizar(idEvento);
				}else if(idEvento == ev_JUEGO){
					juego_tratar_evento(idEvento, auxData);
				}
		}else{
			power_hal_wait();
					
		}
       
    }
}



