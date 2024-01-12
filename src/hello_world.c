
#include "hello_world.h"

static void (*callback_fifo_encolar)();
static int GPIO_HELLO_WORLD_BITS;
static uint32_t cont;

//actualiza el contador 
void hello_world_tick_tack(){
		
    cont= cont+1;											//aumenta el contador
	if(cont >= pow(2,GPIO_HELLO_WORLD_BITS)){
		cont=1;
	}
}

//configura los pines correspondientes y activa la alarma para que encole el evento cada periodo ms
void hello_world_inicializar(GPIO_HAL_PIN_T gpio_inicial, uint8_t num_bits,  enum gpio_hal_pin_dir_t direccion, void (*callback_fifo_encolar_param)(), int GPIO_HELLO_WORLD_BITS_PARAM){
	uint32_t periodo;	
	cont=1;
	GPIO_HELLO_WORLD_BITS = GPIO_HELLO_WORLD_BITS_PARAM;

	
    gpio_hal_sentido(gpio_inicial, num_bits, direccion);	//configurar direccion de los pines

	//programa una alarma para que encole evento cada periodo ms
	periodo= 0x8000000a;	
	alarma_activar(ev_LATIDO,periodo,0);					//activa la alarma para que encole el evento ec_LATIDO cada periodo ms
	
	callback_fifo_encolar = callback_fifo_encolar_param;
     
    
}

//aumenta el contador y encola el evento para visualizar el valor
void hello_world_tratar_evento(){
	hello_world_tick_tack(); 								// aumenta el contador
	callback_fifo_encolar(ev_VISUALIZAR_HELLO,cont); 		// encola el evento para visualizar y el valor a visualizar
}
