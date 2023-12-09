#include "fifo.h"


#define TAM 32


struct fifo{
    EVENTOS queue[TAM]; 
    int index_begin, index_end; // begin = eventos procesados  end= eventos encolados
    GPIO_HAL_PIN_T pin_overflow;
    int adelantado; // booleano que indica si los inidices estan adelantados o no
    uint32_t contador[NUMEVENTOS]; //contador para cada tipo de evento

};

static int GPIO_OVERFLOW_BITS; 



// index_begin = 29 e index_end 3, adelantdo = true -> se encolan 3 index_begin = 0 index_end 3, adelantado = 0
struct fifo cola;
//Inicializaci�n de la cola. Se le pasa como par�metro el pin del GPIO utilizado para marcar errores.
void FIFO_inicializar(GPIO_HAL_PIN_T pin_overflow, int GPIO_OVERFLOW_BITS_PARAM){
		int i = 0;
		GPIO_OVERFLOW_BITS = GPIO_OVERFLOW_BITS_PARAM;
    cola.index_begin = 0;
    cola.index_end = 0;
    cola.pin_overflow = pin_overflow;
    cola.adelantado = 0;
	gpio_hal_sentido(pin_overflow, GPIO_OVERFLOW_BITS, GPIO_HAL_PIN_DIR_OUTPUT);
    
    while (i<=TAM){
        cola.queue[i].id = VOID;
		cola.queue[i].auxData = 0;
        i++;
    }
    i = 0;
    while (i<=NUMEVENTOS){
        cola.contador[i] = 0; //inicializar contadores
        i++;
    }

    

}

//Esta funci�n guardar� en la cola el evento. El campo ID_evento, que permita identificar el evento (p.e. qu� interrupci�n ha saltado) 
//y el campo auxData en caso de que el evento necesite pasar informaci�n extra.
void FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData){
    uint32_t irq;
    irq = read_IRQ_bit();
    
    if(irq == 0){
       disable_irq();
    }    
    
    if(cola.adelantado == 1 && cola.index_begin == cola.index_end){

        gpio_hal_escribir( cola.pin_overflow, GPIO_OVERFLOW_BITS,  GPIO_HAL_PIN_DIR_OUTPUT);
    }
    cola.queue[cola.index_end].id = ID_evento;
   cola.queue[cola.index_end].auxData = auxData;
		    cola.index_end = (cola.index_end +1) % TAM;
		if(cola.index_end == 0){
        cola.adelantado = 1;
    }

    cola.contador[ID_evento]++; //aumenta contador
    if(irq == 0){
       enable_irq();
    }    
  
}

//Si hay eventos sin procesar, devuelve un valor distinto de cero y el evento m�s antiguo sin procesar por referencia. Cero indicar� 
//que la cola est� vac�a y no se ha devuelto ning�n evento.
uint8_t FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData){
		uint32_t irq;
		irq = read_IRQ_bit();
		if(irq == 0){
			 disable_irq();
		} 
    if(cola.index_begin != cola.index_end ){
  
        *ID_evento = (cola.queue[cola.index_begin].id);
        *auxData = (cola.queue[cola.index_begin].auxData);
			cola.index_begin = (cola.index_begin +1)% TAM;
        
        if(cola.index_begin == 0){
            cola.adelantado = 0;
        }
        if(irq == 0){
            enable_irq();
        }
        
        return 1;
    }else{
        if(irq == 0){
            enable_irq();
        }
   
        return 0;
    }
}

//Dado un identificador de evento nos devuelve el n�mero total de veces que ese evento se ha encolado. El evento VOID nos devolver� 
//el total de eventos encolados desde el inicio.
uint32_t FIFO_estadisticas(EVENTO_T ID_evento){
    
   if (ID_evento == VOID)
   {
    uint32_t cont = 0;
    int i = 0;
    while (i < NUMEVENTOS){
        cont = cont + cola.contador[i];
        i++;
    }
    return cont;
   }else{
    
    
    return cola.contador[ID_evento];
   }
   
    
}

//Vacía la información de las estádisticas de todos los eventos de la cola fifo
void FIFO_reiniciar_estadisticas(){
    
    int i = 0;
    while(i < NUMEVENTOS){
        cola.contador[i] = 0;
        i++;
    }
}



