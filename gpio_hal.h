#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <LPC210X.H>
#include <inttypes.h>

	
typedef uint8_t GPIO_HAL_PIN_T;

enum  gpio_hal_pin_dir_t{
	GPIO_HAL_PIN_DIR_INPUT,
	GPIO_HAL_PIN_DIR_OUTPUT
};




///Permite emplear el GPIO y debe ser invocada
//antes de poder llamar al resto de funciones de la biblioteca.
__inline static
void gpio_hal_iniciar(){

    PINSEL0 = 0;
    PINSEL1 = 0;

}

//los bits indicados se utilizar?n como entrada o salida seg?n la direcci?n.
__inline static void 
gpio_hal_sentido( GPIO_HAL_PIN_T gpio_inicial, uint8_t num_bits,  enum gpio_hal_pin_dir_t direccion){
	
    if(direccion == GPIO_HAL_PIN_DIR_OUTPUT){
	    IODIR = IODIR | (~(0xFFFFFFFF << num_bits) << gpio_inicial);
    }else{
	    IODIR = IODIR &  (~(~(0xFFFFFFFF << num_bits) << gpio_inicial));
        
    }
}

//gpio_inicial indica el primer bit a leer, num_bits indica
//cu?ntos bits queremos leer. La funci?n devuelve un entero con el valor
//de los bits indicados
__inline static uint32_t
gpio_hal_leer( GPIO_HAL_PIN_T gpio_inicial, uint8_t num_bits){
	
	return (IOPIN & (~(0xFFFFFFFF << num_bits) << gpio_inicial)) >> gpio_inicial ;
	
}

//similar al anterior, pero en lugar de leer escribe en los
//bits indicados el valor (si valor no puede representarse en los bits
//indicados se escribir? los num_bits menos significativos a partir del inicial)
__inline static void 
gpio_hal_escribir( GPIO_HAL_PIN_T bit_inicial, uint8_t num_bits, uint32_t valor){

		IOSET = valor << bit_inicial & ((~(0xFFFFFFFF << num_bits)) << bit_inicial);	
		IOCLR = ~valor << bit_inicial  & ((~(0xFFFFFFFF << num_bits)) << bit_inicial);
		
}

#endif
