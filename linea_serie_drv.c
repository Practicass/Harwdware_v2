

#include "linea_serie_drv.h"

static const int MAXBUFFER = 3;
static const int MAXBUFFERSALIDA = 600;

static volatile uint8_t bufferEntrada[MAXBUFFER];
static  int numEscritura = 0;
static  int almacenar = 0;

static void (*callback_fifo_encolar)();
static void (*callback_gpio_hal_sentido)();
static void (*callback_gpio_hal_escribir)();
static uint32_t (*callback_gpio_hal_leer)();
static volatile uint8_t evento_id_ev_RX_SERIE;
static volatile uint8_t evento_id_ev_TX_SERIE;
static volatile uint8_t GPIO_SERIE_ERROR;
static volatile uint8_t GPIO_SERIE_ERROR_BITS;
static volatile uint8_t GPIO_HAL_PIN_DIR_OUTPUT;
static uint8_t bufferSalida[MAXBUFFERSALIDA];
static int contadorEscritura = 1; 
static unsigned int contadorAux = 0; 


//inicializa las estructuras de datos, inicializa linea_serie_hal y asigna los valores de los pines y de los eventos
void linea_serie_drv_inicializar(void (*callback_fifo_encolar_param)(), uint8_t id_ev_RX_SERIE, uint8_t id_ev_TX_SERIE, void (*callback_gpio_hal_sentido_param)(), void (*callback_gpio_hal_escribir_param)(), uint32_t (*callback_gpio_hal_leer_param)(), int GPIO_SERIE_ERROR_PARAM, int GPIO_SERIE_ERROR_BITS_PARAM, int GPIO_HAL_PIN_DIR_OUTPUT_PARAM){
    callback_fifo_encolar = callback_fifo_encolar_param;
	evento_id_ev_RX_SERIE = id_ev_RX_SERIE;
    evento_id_ev_TX_SERIE = id_ev_TX_SERIE;
    GPIO_SERIE_ERROR = GPIO_SERIE_ERROR_PARAM;
    GPIO_SERIE_ERROR_BITS = GPIO_SERIE_ERROR_BITS_PARAM;
    GPIO_HAL_PIN_DIR_OUTPUT = GPIO_HAL_PIN_DIR_OUTPUT_PARAM;
    callback_gpio_hal_sentido = callback_gpio_hal_sentido_param;
    callback_gpio_hal_escribir = callback_gpio_hal_escribir_param;
    callback_gpio_hal_leer = callback_gpio_hal_leer_param;
    callback_gpio_hal_sentido(GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS, GPIO_HAL_PIN_DIR_OUTPUT);
    linea_serie_hal_inicializar(linea_serie_drv_leer,linea_serie_drv_continuar_envio);
}

//lee el caracter introducido por el usuario y lo almacena en el buffer de entrada, comprueba que el caracter 
//introducido es de formato correcto y genera el evento ev_RX_SERIE
void linea_serie_drv_leer(uint8_t caracter){
    if(caracter == '$'){                                        // comenzara a almacenar en la siguiente interrupcion
        almacenar = 1;                                          // almacenar = true y por tanto permite almacenar los caracteres introducidos                       
        numEscritura = 0;                                       // se inicializa el indice de escritura a 0
        //apagar led error si esta encedido
        if(callback_gpio_hal_leer(GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS) == 1){
            callback_gpio_hal_escribir(GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS, 0);
        }
        
    }else if(almacenar && numEscritura == 3 && caracter == '!'){ // si se recibe el caracter '!' se para de alamacenar
    
        almacenar = 0;                                          // almacenar = false y por tanto no permite almacenar los caracteres introducidos 
        numEscritura = 0;                                       // se reinicia el indice de escritura a 0  

        // se genera el evento ev_RX_SERIE con los 3 caracteres almacenados en el buffer de entrada           
        callback_fifo_encolar(evento_id_ev_RX_SERIE, (bufferEntrada[0] << 16) | (bufferEntrada[1] << 8) | bufferEntrada[2]); 
        
    }else if(numEscritura >= 3){ // se han introducio más caracteres de los posibles
        almacenar = 0;                                          // almacenar = false y por tanto no permite almacenar los caracteres introducidos 
        //encender led error
        callback_gpio_hal_escribir(GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS, 1);

    }else if(almacenar){ // se almacena el caracter introducido 
        bufferEntrada[numEscritura] = caracter;                    // se almacena el caracter en el buffer de entrada
        numEscritura++;                                            // se incrementa el indice de escritura
    }
}

//inicializa el buffer de salida y envia el primer caracter
void linea_serie_drv_enviar_array(uint8_t cadena[]){
    
    // se almacena la cadena en el buffer de salida
    contadorAux = 0;
		while(cadena[contadorAux] != '\0'){                      // se recorre la cadena hasta encontrar el caracter '\0' (fin de cadena)
			bufferSalida[contadorAux] = cadena[contadorAux];
			contadorAux++;
		}
		
   
    linea_serie_drv_escribir(bufferSalida[0]);                  // se envia el primer caracter
    

    
}
//envia el siguiente caracter del buffer de salida, si se ha enviado el ultimo caracter se genera el evento ev_TX_SERIE
void linea_serie_drv_continuar_envio(){

    if(contadorEscritura < contadorAux){
		linea_serie_drv_escribir(bufferSalida[contadorEscritura]);
        contadorEscritura++;                                    // se incrementa el indice de escritura
    }
    else{
        // se genera el evento ev_TX_SERIE
        callback_fifo_encolar(evento_id_ev_TX_SERIE,0);
        contadorEscritura=1;                                    // se reinicia el indice de escritura
		contadorAux = 0;                                        // se reinicia el contador auxiliar                        
    }
    

}

//envia el caracter introducido por el usuario a la linea serie
void linea_serie_drv_escribir(uint8_t caracter){
		linea_serie_hal_escribir(caracter);
}
