
#include "visualizar.h"

static int GPIO_HELLO_WORLD;
static int GPIO_HELLO_WORLD_BITS;

void visualizar_inicializar(int GPIO_HELLO_WORLD_PARAM, int GPIO_HELLO_WORLD_BITS_PARAM){
    gpio_hal_sentido(16,8,GPIO_HAL_PIN_DIR_OUTPUT);
	GPIO_HELLO_WORLD  = GPIO_HELLO_WORLD_PARAM;
	GPIO_HELLO_WORLD_BITS = GPIO_HELLO_WORLD_BITS_PARAM;
}

void visualizarCuenta(uint32_t cuenta){
    gpio_hal_escribir(16,8,cuenta);
}

void visualizarHello(uint32_t led){
    gpio_hal_escribir(GPIO_HELLO_WORLD,GPIO_HELLO_WORLD_BITS, led);
}


