

#include "linea_serie_drv.h"

const int MAXBUFFER = 3;
const int MAXBUFFERSALIDA = 800;

static volatile uint8_t bufferEntrada[MAXBUFFER];
static  int numEscritura = 0;
static  int almacenar = 0;

static void (*callback_fifo_encolar)();
static volatile uint8_t evento_id_ev_RX_SERIE;
static volatile uint8_t evento_id_UART0_CARACTER;
static volatile uint8_t evento_id_CONTINUAR_ENVIO;
static volatile uint8_t evento_id_ev_TX_SERIE;
static uint8_t bufferSalida[MAXBUFFERSALIDA];
static int contadorEscritura = 1; 
static unsigned int contadorAux = 0; 



void linea_serie_drv_inicializar(void (*callback_fifo_encolar_param)(), uint8_t id_ev_RX_SERIE, uint8_t id_UART0_CARACTER, uint8_t id_CONTINUAR_ENVIO, uint8_t id_ev_TX_SERIE){
    callback_fifo_encolar = callback_fifo_encolar_param;
		evento_id_ev_RX_SERIE = id_ev_RX_SERIE;
        evento_id_UART0_CARACTER = id_UART0_CARACTER;
        evento_id_CONTINUAR_ENVIO = id_CONTINUAR_ENVIO;
        evento_id_ev_TX_SERIE = id_ev_TX_SERIE;
    //gpio_hal_sentido(GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS, GPIO_HAL_PIN_DIR_OUTPUT);
    linea_serie_hal_inicializar(linea_serie_drv_leer,linea_serie_drv_continuar_envio);
}

void linea_serie_drv_leer(uint8_t caracter){
    if(caracter == '$'){ // comenzar� a almacenar en la siguiente interrupci�n
        almacenar = 1;
        numEscritura = 0;
       // if(gpio_hal_leer(GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS) == 1){
        //    gpio_hal_escribir(GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS, 0);
       // }
        
    }else if(almacenar && numEscritura == 3 && caracter == '!'){ // se recibe el caracter ! y se para de alamacenar
    
        almacenar = 0;
        numEscritura = 0;
        
        //if((bufferEntrada[0] == 'E' && bufferEntrada[1] == 'N'&& bufferEntrada[2] == 'D')||(bufferEntrada[0] == 'N' && bufferEntrada[1] == 'E'&& bufferEntrada[2] == 'W')||(bufferEntrada[0] == 'T' && bufferEntrada[1] == 'A'&& bufferEntrada[2] == 'B')||
				//	(bufferEntrada[0] >= '1' && bufferEntrada[0] <= '7' && bufferEntrada[1] == '-' && bufferEntrada[2] >= '1' && bufferEntrada[2] <= '7')){
            callback_fifo_encolar(evento_id_ev_RX_SERIE, (bufferEntrada[0] << 16) | (bufferEntrada[1] << 8) | bufferEntrada[2]);
        //}

    }else if(numEscritura >= 3){ // se han introducio m�s caracteres de los posibles
        almacenar = 0;
        //encender led error
       // gpio_hal_escribir(GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS, 1);

    }else if(almacenar){ // se almacena el caracter introducido 
        bufferEntrada[numEscritura] = caracter;
        numEscritura++;
    }
}

void linea_serie_drv_enviar_array(uint8_t cadena[]){
    // falta for
    // se separa la cadena entera en sus caracteres
		while(cadena[contadorAux] != '%'){
			bufferSalida[contadorAux] = cadena[contadorAux];
			contadorAux++;
		}
		
    
    // generar evento que llame a linea_serie_hal
    linea_serie_drv_escribir(bufferSalida[0]);    
    

    
}

void linea_serie_drv_continuar_envio(){

    if(contadorEscritura < contadorAux){
        //callback_fifo_encolar(evento_id_UART0_CARACTER, bufferSalida[contadorEscritura]);
			linea_serie_drv_escribir(bufferSalida[contadorEscritura]);
        contadorEscritura++;
    }
    else{
        // se genera el evento ev_TX_SERIE
        callback_fifo_encolar(evento_id_ev_TX_SERIE,0);
        contadorEscritura=1;
		contadorAux = 0;
    }
    

}

void linea_serie_drv_escribir(uint8_t caracter){
		linea_serie_hal_escribir(caracter);
}
