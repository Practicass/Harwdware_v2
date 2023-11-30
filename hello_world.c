
#include "hello_world.h"

static void (*callback_fifo_encolar)();
static int GPIO_HELLO_WORLD_BITS;
static uint32_t contHelloWorld;

//actualiza el contador y el estado de los leds 
void hello_world_tick_tack(){

		//aumenta el contador
    contHelloWorld= contHelloWorld+1;
		if(contHelloWorld >= pow(2,GPIO_HELLO_WORLD_BITS)){
			contHelloWorld=1;
		}
			
		
		
}

//configura los pines correspondientes
void hello_world_inicializar(GPIO_HAL_PIN_T gpio_inicial, uint8_t num_bits,  enum gpio_hal_pin_dir_t direccion, void (*callback_fifo_encolar_param)(), int GPIO_HELLO_WORLD_BITS_PARAM){
	uint32_t periodo;	
	contHelloWorld=1;
	GPIO_HELLO_WORLD_BITS = GPIO_HELLO_WORLD_BITS_PARAM;
		
	
		
	
		//configurar direccion de los pines
    gpio_hal_sentido(gpio_inicial, num_bits, direccion);

		//prgramar el reloj para que encole evento cada periodo ms
		periodo= 0x8000000a;
		
    //temporizador_drv_reloj(periodo, func, ev_LATIDO);
	alarma_activar(ev_LATIDO,periodo,0);
		//llamar a alarma
	
	callback_fifo_encolar = callback_fifo_encolar_param;
     
    
}


void hello_world_tratar_evento(){
	
	hello_world_tick_tack(); // aumenta el contador
	callback_fifo_encolar(ev_VISUALIZAR_HELLO,contHelloWorld); // encola el evento para visualizar y el valor a visualizar
}
