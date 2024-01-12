
#ifndef FIFO_H
#define FIFO_H
#include <inttypes.h>
#include "gpio_hal.h"
#include "eventos.h"
#include "swi.h"




//Inicializacion de la cola. Se le pasa como parametro el pin del GPIO utilizado para marcar errores.
void FIFO_inicializar(GPIO_HAL_PIN_T pin_overflow, int GPIO_OVERFLOW_BITS); 

//Esta funcion guardara en la cola el evento. El campo ID_evento, que permita identificar el evento (p.e. que interrupcion ha saltado) 
//y el campo auxData en caso de que el evento necesite pasar informacion extra.
void FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData); 

//Si hay eventos sin procesar, devuelve un valor distinto de cero y el evento mas antiguo sin procesar por referencia. Cero indicara 
//que la cola esta vacia y no se ha devuelto ningun evento.
uint8_t FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData);

//Dado un identificador de evento nos devuelve el numero total de veces que ese evento se ha encolado. El evento VOID nos devolvera 
//el total de eventos encolados desde el inicio.
uint32_t FIFO_estadisticas(EVENTO_T ID_evento);

//Vacía la información de las estadisticas de todos los eventos de la cola fifo
void FIFO_reiniciar_estadisticas(void);





#endif



