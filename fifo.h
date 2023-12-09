
#ifndef FIFO_H
#define FIFO_H
#include <inttypes.h>
#include "gpio_hal.h"
#include "eventos.h"
#include "swi.h"




//Inicializaci?n de la cola. Se le pasa como par?metro el pin del GPIO utilizado para marcarerrores.
void FIFO_inicializar(GPIO_HAL_PIN_T pin_overflow, int GPIO_OVERFLOW_BITS); 

//Esta funci?n guardar? en la cola el evento. El campo ID_evento, que permita identificar el evento (p.e. qu? interrupci?n ha saltado) 
//y el campo auxData en caso de que el evento necesite pasar informaci?n extra.
void FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData); 

//Si hay eventos sin procesar, devuelve un valor distinto de cero y el evento m?s antiguo sin procesar por referencia. Cero indicar? 
//que la cola est? vac?a y no se ha devuelto ning?n evento.
uint8_t FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData);

//Dado un identificador de evento nos devuelve el n?mero total de veces que ese evento se ha encolado. El evento VOID nos devolver? 
//el total de eventos encolados desde el inicio.
uint32_t FIFO_estadisticas(EVENTO_T ID_evento);


void FIFO_reiniciar_estadisticas(void);





#endif



