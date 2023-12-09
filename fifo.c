#include "fifo.h"


#define TAM 32

//Estructura de la cola. 
struct fifo{
    EVENTOS queue[TAM];                     // cola de eventos
    int index_begin, index_end;             // indices de la cola, begin = eventos procesados  end= eventos encolados            
    GPIO_HAL_PIN_T pin_overflow;            // pin del GPIO utilizado para marcar errores
    int adelantado;                         // booleano que indica si los inidices estan adelantados o no
    uint32_t contador[NUMEVENTOS];          //contador para cada tipo de evento

};

static int GPIO_OVERFLOW_BITS;              // bits del GPIO utilizado para marcar errores




struct fifo cola;                           // cola de eventos

//Inicializacion de la cola. Se le pasa como parametro el pin del GPIO utilizado para marcar errores.
void FIFO_inicializar(GPIO_HAL_PIN_T pin_overflow, int GPIO_OVERFLOW_BITS_PARAM){
    //inicializar variables
	int i = 0;
	GPIO_OVERFLOW_BITS = GPIO_OVERFLOW_BITS_PARAM;
    cola.index_begin = 0;
    cola.index_end = 0;
    cola.pin_overflow = pin_overflow;
    cola.adelantado = 0;
    //configurar pin de error
	gpio_hal_sentido(pin_overflow, GPIO_OVERFLOW_BITS, GPIO_HAL_PIN_DIR_OUTPUT);
    
    //inicializar cola
    while (i<=TAM){
        cola.queue[i].id = VOID;
		cola.queue[i].auxData = 0;
        i++;
    }
    i = 0;
    //inicializar contadores
    while (i<=NUMEVENTOS){
        cola.contador[i] = 0; 
        i++;
    }

    

}

//Esta funcion guardara en la cola el evento. El campo ID_evento, que permita identificar el evento (p.e. que interrupcion ha saltado) 
//y el campo auxData en caso de que el evento necesite pasar informacion extra.
void FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData){
    uint32_t irq;
    irq = read_IRQ_bit();                            //guarda el estado de las interrupciones
    
    //deshabilita las interrupciones unicamente si las interrupciones estan habilitadas
    if(irq == 0){
       disable_irq();
    }    
    
    //si la cola esta llena, se marca el error
    if(cola.adelantado == 1 && cola.index_begin == cola.index_end){

        gpio_hal_escribir( cola.pin_overflow, GPIO_OVERFLOW_BITS,  GPIO_HAL_PIN_DIR_OUTPUT);
    }
    cola.queue[cola.index_end].id = ID_evento;      //guarda el evento
    cola.queue[cola.index_end].auxData = auxData;   //guarda el auxData
	cola.index_end = (cola.index_end +1) % TAM;     //aumenta el indice de la cola
    //si el indice de la cola llega al final, se marca el booleano adelantado
	if(cola.index_end == 0){
        cola.adelantado = 1;
    }

    cola.contador[ID_evento]++;                     //aumenta contador
    //restablece el estado de las interrupciones
    if(irq == 0){
       enable_irq();
    }    
  
}

//Si hay eventos sin procesar, devuelve un valor distinto de cero y el evento mas antiguo sin procesar por referencia. Cero indicara 
//que la cola esta vacia y no se ha devuelto ningun evento.
uint8_t FIFO_extraer(EVENTO_T *ID_evento, uint32_t* auxData){
	uint32_t irq;
	irq = read_IRQ_bit();                                   //guarda el estado de las interrupciones
    //deshabilita las interrupciones unicamente si las interrupciones estan habilitadas
	if(irq == 0){
		disable_irq();
	} 
    if(cola.index_begin != cola.index_end ){
  
        *ID_evento = (cola.queue[cola.index_begin].id);     //devuelve el evento
        *auxData = (cola.queue[cola.index_begin].auxData);  //devuelve el auxData
			cola.index_begin = (cola.index_begin +1)% TAM;  //aumenta el indice de la cola
        
        //si el indice de la cola llega al final, se modifica el valor del booleano adelantado
        if(cola.index_begin == 0){
            cola.adelantado = 0;
        }
        //restablece el estado de las interrupciones
        if(irq == 0){
            enable_irq();
        }
        
        return 1;                                           //devuelve 1 si se ha extraido un evento
    }else{
        //restablece el estado de las interrupciones
        if(irq == 0){
            enable_irq();
        }
   
        return 0;                                           //devuelve 0 si no se ha extraido ningun evento al estar la cola vacia                           
    }
}

//Dado un identificador de evento nos devuelve el numero total de veces que ese evento se ha encolado. El evento VOID nos devolvera 
//el total de eventos encolados desde el inicio.
uint32_t FIFO_estadisticas(EVENTO_T ID_evento){

    //si el evento es VOID, devuelve el total de eventos encolados
    if (ID_evento == VOID){
        uint32_t cont = 0;
        int i = 0;
        //suma los contadores de todos los eventos
        while (i < NUMEVENTOS){
            cont = cont + cola.contador[i];
            i++;
        }
        return cont;                                        //devuelve el total de eventos encolados
   }else{
    //si el evento no es VOID, devuelve el numero de veces que ese evento se ha encolado
        return cola.contador[ID_evento];
   }
   
    
}

//Vacía la información de las estadisticas de todos los eventos de la cola fifo
void FIFO_reiniciar_estadisticas(){
    
    int i = 0;
    //recorre los contadores de todos los eventos
    while(i < NUMEVENTOS){
        cola.contador[i] = 0;                       //pone a 0 todos los contadores 
        i++;
    }
}



