
#include "juego.h"

static volatile int cuenta;								//veces que se han pulsado los botones (practica 2b)
static volatile int intervalo;							//tiempo (practica 2b)
static volatile int ultimaPulsacion = 0;				//momento en el que se pulsa por última vez(practica 2b)
static volatile int primeraVez = 0;						//booleano que indica si es la primera vez que se pulsa un boton (practica 2b)
static int GPIO_JUGAR_ERROR;							//valor del pin de error jugada incorrecta
static int GPIO_JUGAR_ERROR_BITS;						//valor del tamaño en bits que corresponden al error 
static void (*callback_gpio_hal_escribir)();			//puntero a la funcion gpio_hal_escribir
static uint32_t (*callback_gpio_hal_leer)();			//puntero a la funcion gpio_hal_leer
static uint32_t (*callback_fifo_estadisticas)();		//puntero a la funcion FIFO_estadisticas
static void (*callback_fifo_reiniciar_estadisticas)();	//puntero a la funcion FIFO_reiniciar_estadisticas
static uint8_t salida[8][8];							//salida del tablero
static TABLERO cuadricula;								//tablero de juego
static uint32_t t1;										//tiempo previo a mostrar el tablero (demostrador practica 2c)
static uint8_t turno = 1;								//indica a que jugador le toca mover ficha
static uint32_t turnoEmpieza = 1; 						//numero del jugador que empieza la siguiente partida
static uint8_t fila = 0;								//valor de la fila introducida por un jugador
static uint8_t columna = 0;								//valor de la columna introducida por un jugador
static uint8_t reason = 0; 								//causa del final de la partida: 0 -> victoria, 1 -> cancelada mediante boton, 2 -> comando end introducido
int indexFinal = 0;
uint8_t bufferMsgFinal[600];

//variables para calcular el tiempo del procesador en una partida
static uint32_t t1Procesador; 							// tiempo de comienzo de la partida
static uint32_t t2Procesador; 							// tiempo de final de partida

//variables para calcular el tiempo de computo de la funcion conecta_k_hay_linea
static uint32_t t1HayLinea; 							//tiempo previo a ejecutar conecta_k_hay_linea
static uint32_t t2HayLinea; 							//tiempo tras ejecutar conecta_k_hay_linea
static uint32_t sumHayLinea = 0;						//tiempo total  que se tarda en ejecutar conecta_k_hay_linea
static uint32_t numHayLinea = 0; 						//numero de veces que se ejecuta conecta_k_hay_linea

//variables para calcular el tiempo que les cuesta a los jugadoresa introducir una jugada
static uint32_t t1Humano;								//tiempo desde que el usuario puede introducir un comando
static uint32_t t2Humano;								//tiempo en el que el usuario introduce un comando  
static uint32_t sumHumano = 0; 							//tiempo total  que tardan los jugadores en introducir un comando
static uint32_t numHumano = 0; 							//numero de veces que los jugadores introducen un comando





// definicion de los estados
enum ESTADOS{ 
	PAG_PRINCIPAL = 0,
    ESCRITURA_PAG_PRINCIPAL = 1,
    WAIT_INICIO_PARTIDA = 2,
    ESCRITURA_MOSTRAR_TABLERO = 3,
    WAIT_COMANDO = 4,
	ESCRITURA_MOSTRAR_ERROR = 5,
	ESCRITURA_MOSTRAR_FIN = 6,
	COMANDO_CORRECTO = 7,
	ESCRITURA_COMANDO_CORRECTO = 8,
	WAIT_CANCELAR = 9,
	CANCELADO = 10,
	REALIZAR_COMANDO = 11,
	ESCRITURA_TABLERO_FIN = 12,
	}

static  state = PAG_PRINCIPAL; // se establece el estado inicial


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES PRIVADAS------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE AUXILIARES ------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie, al final del buffer se añade los caracteres '\n' y '\0'
void convesor_entero_char(uint32_t num, uint8_t array_digitos[]){
	int numAux = num;									//realizamos una copia del array para no modificarlo
	//calculamos la longitud del vector
	unsigned int longitud = 0;							
	while (numAux != 0) {
        numAux /= 10;
        longitud++;
    }
    // Crear un array para almacenar los dígitos
    numAux = num;
	array_digitos[longitud] = '\n';						//en la última posicion del vector colocamos el salto de linea y nuestro caracter de fin de buffer
	array_digitos[longitud+1] = '\0';
    // Separar cada dígito y almacenarlo en el array
    for (int i = longitud - 1; i >= 0; i--) {			//copiamos cada digito en el array
        array_digitos[i] = (numAux % 10) + '0';
        numAux /= 10;
    }
	
}

//muestra el tiempo que tarda en mostrar el tablero, el cual ha sido previamente calculado
void conecta_K_visualizar_tiempo(uint32_t num){
	uint8_t array_digitos[100];
	convesor_entero_char(num,array_digitos);
	linea_serie_drv_enviar_array(array_digitos);
}

//calcula el tiempo que tarda en mostrar el tablero (practica 2c)
void tiempo_visualizar_tablero(uint32_t t2){

	uint32_t t3; 										// t3 = diferencia entre el tiempo previo a visualizar el tablero y el tiempo tras su visualización
	t3 = t2-t1;
	conecta_K_visualizar_tiempo(t3);
}

//rellena el buffer 1 con el contenido de buffer 2 a partir de index. 
//Devuelve el indice de la última compononete + 1 que ha sido rellenada
int concatenar_array(uint8_t buffer1[], uint8_t buffer2[], int index){
	int j=0;											//variable auxiliar del bucle
	while(buffer2[j] != '\0'){							//mientras el carcter es distinto al caracter de finalización del buffer '\0'
		buffer1[index+j] = buffer2[j];					//se copia la componente j en la componente correpondiente en el buffer1
		j++;
	}
	return index+j;										//se devuelve el siguiente indice al ultimo que ha sido relleneado
}






//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie, al final del buffer no se añade los caracteres '\n' y '\0' y devuelve la longitud del numero
int convesor_entero_char_2(uint32_t num, uint8_t array_digitos[], int indice){
	int numAux = num;									//realizamos una copia del array para no modificarlo
	//calculamos la longitud del vector
	unsigned int longitud = 0;
	while (numAux != 0) {
        numAux /= 10;
        longitud++;
    }
    // Crear un array para almacenar los dígitos
    numAux = num;

    // Separar cada dígito y almacenarlo en el array
    for(int i = longitud - 1 + indice;i >= indice;i--){	//copiamos cada digito en el array
        array_digitos[i] = (numAux % 10) + '0';
        numAux /= 10;
    }
	
	return longitud;									//devuelve la longitud del número
}

//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie, al final del buffer se añade el caracterer '\0' 
//y si num = 0 devuelve el caracter 0. Ademas devuelve la longitud del array
int convesor_entero_char_3(uint32_t num, uint8_t array_digitos[], int indice){
	int numAux = num;									//realizamos una copia del array para no modificarlo
	unsigned int longitud = 0;
	//si el numero es 0 la longuitud es 1 digito
	if(num == 0){
		longitud = 1;
	}else{
		//sino es 0, se calcula la longitud
		while (numAux != 0) {
        	numAux /= 10;
        	longitud++;
    	}
	}

    // Crear un array para almacenar los dígitos
    numAux = num;

    // Separar cada dígito y almacenarlo en el array
    for(int i = longitud - 1 + indice;i >= indice;i--){	//copiamos cada digito en el array
        array_digitos[i] = (numAux % 10) + '0';
        numAux /= 10;
    }
	array_digitos[longitud] = '\0';
	return longitud+1;									//devuelve la longitud del array
}




//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE VISUALIZACION TABLERO--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//convierte la matriz salida que hay en memoria en un vector y devuelve la posición siguiente al último elemento del vector
int tablero_to_array(uint8_t bufferTablero[]){
	int i=0;
	int j=0;
	int aux = 0;												//variable booleana, si su contenido es 0 introducirá un caracter del marco o del contenido del tablero, si su contenido es 1 se introducirá una barra de separación

	while(i < 8){ 												//para cada fila del tablero que se va a introducir en bufferTablero
		j=0;
		while(j<8 || j==8 && aux == 1){							//para cada columna del tablero
			if(i==0){											//si corresponde al marco superior
				if(aux == 0){
					bufferTablero[j*2] = '0' + j;				//se introduce caracter del marco
					aux=1;
				}else{
					bufferTablero[j*2+aux] = '|';				//se introduce barra de separación
					aux=0;
					j++;
				}
			}else if(j==0){										//si corresponde al marco lateral izquierdo
				if(aux % 2 == 0){
					bufferTablero[i*8*2+j*2+i] = '0' + i;		//se introduce caracter del marco
					aux=1;
				}else{
					bufferTablero[i*8*2+j*2+aux+i] = '|';		//se introduce barra de separación
					aux=0;
					j++;
				}	
			}else{												//si se trata del contenido del tablero
				if(aux == 0){
					if(salida[i][j] == 0x11){
						bufferTablero[i*8*2+j*2+i] = 'B';		//se introduce ficha blanca
					}else if(salida[i][j] == 0x22){
						bufferTablero[i*8*2+j*2+i] = 'N';		//se introduce ficha negra
					}else{
						bufferTablero[i*8*2+j*2+i] = ' ';		//se deja vacia la celda
					}	
					aux=1;	
				}else{
					bufferTablero[i*8*2+j*2+aux+i] = '|';		//se introduce barra de separación
					aux=0;
					j++;
				}
			}
		}
		bufferTablero[i*8*2+j*2+i] = '\n';
		i++;
	}
	return i*8*2+i;												//se devuelve la siguiente posicion al ultimo elemento introducido en el vector
}




//muestra el tablero final y muestra el número del jugador que ha ganado
void conecta_K_visualizar_tablero_ganador(){
	uint8_t bufferTablero[300];
	t1 = clock_get_us();								//se obtiene el tiempo actual
	conecta_K_visualizar_tablero(&cuadricula, salida);	//se visualiza el tablero
	
	int ind = tablero_to_array(bufferTablero);
	uint8_t bufferAux[42] = "Ganador:  \n***************************\n\n\0";
	bufferAux[9] = turno +'0';							//se introduce el numero del jugador ganador
	
	ind = concatenar_array(bufferTablero, bufferAux, ind);

	bufferTablero[ind] = '\0';							//se introduce el caracter de fin de buffer
	linea_serie_drv_enviar_array(bufferTablero);
}



//muestra el estado actual del tablereo y el turno del jugador que le corresponde
void conecta_K_visualizar_tablero_juego(){
	uint8_t bufferTablero[300];
	t1 = clock_get_us();								//se obtiene el tiempo actual
	
	conecta_K_visualizar_tablero(&cuadricula, salida);	//se visualiza el tablero
	
	int ind = tablero_to_array(bufferTablero);
	uint8_t bufferAux[40] = "Turno:  \n***************************\n\n\0";
	bufferAux[7] = turno +'0';							//se introduce el numero del jugador que le toca mover
	ind = concatenar_array(bufferTablero, bufferAux, ind);
	bufferTablero[ind] = '\0';							//se introduce el caracter de fin de buffer	
	linea_serie_drv_enviar_array(bufferTablero);
}


//muestra el movimiento introducido por un jugador y el mensaje informando sobre la cancelacion de este
void conecta_K_visualizar_movimiento_juego(){// se puede llamar a una funcion nueva que sea parecida a conecta k visualizar tablero juego

	uint8_t bufferTablero[300];

	conecta_K_visualizar_tablero(&cuadricula, salida);

	int ind = tablero_to_array(bufferTablero);
	//se introduce el caracter correspondiente al jugador que ha realizado el movimiento
	if(turno==1){									
		bufferTablero[fila*8*2+columna*2+fila] = 'B';
	}else if(turno ==2){
		bufferTablero[fila*8*2+columna*2+fila] = 'N' ;
	}
	//se muestra el mensaje de movimiento cancelado
	uint8_t bufferAux[65] = "PULSA EL BOTON 1 PARA CANCELAR\n***************************\n\n\0";
	ind = concatenar_array(bufferTablero, bufferAux, ind);
	bufferTablero[ind] = '\0';							//se introduce el caracter de fin de buffer	
	linea_serie_drv_enviar_array(bufferTablero);

}


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE VISUALIZACION TEXTO--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//introduce en el buffer el titulo de partida fin y actualiza el nuevo indice del buffer 
void mostrar_titulo_final_juego(){ //0 -> victoria, 1 -> cancel, 2 -> end
	
	//depende de la razon que ha acabado la partida se muestra un mensaje u otro
	if (reason == 1 || reason == 2){
		uint8_t bufferMsgFinJuego[38] = "FIN DE LA PARTIDA\nPARTIDA CANCELADA\n\0";
		indexFinal = concatenar_array(bufferMsgFinal,bufferMsgFinJuego,indexFinal);
	}else if( reason == 0){
		uint8_t bufferMsgFinJuego[39] = "FIN DE LA PARTIDA\nPARTIDA FINALIZADA\n\0";
		indexFinal = concatenar_array(bufferMsgFinal,bufferMsgFinJuego,indexFinal);

	}
	
	
}

//introduce en el buffer el motivo de finalización de partida y actualiza el nuevo indice del buffer 
void mostrar_causa(){
	
	//depende de la razon que ha acabado la partida se muestra un mensaje u otro
	if(reason == 0){
		uint8_t bufferMsg[32] = "CAUSA: VICTORIA DEL JUGADOR  \n\0";
		bufferMsg[28] = turno +'0';						//se introduce el numero del jugador ganador
		indexFinal = concatenar_array(bufferMsgFinal,bufferMsg,indexFinal);

	}else if(reason == 1){
		uint8_t bufferMsg[25] = "CAUSA: BOTON 2 PULSADO\n\0";
		indexFinal = concatenar_array(bufferMsgFinal,bufferMsg,indexFinal);

	}else if(reason == 2) {
		uint8_t bufferMsg[33] = "CAUSA: COMANDO END INTRODUCIDO\n\0";
		indexFinal = concatenar_array(bufferMsgFinal,bufferMsg,indexFinal);
	
	}
	

	
}

//introduce en el buffer el tiempo total del uso del procesador y actualiza el nuevo indice del buffer 
void mostrar_tiempo_procesador(){
	uint32_t tTotalProcesador = t2Procesador -t1Procesador;			//se calcula el tiempo total de uso del procesador
	uint8_t bufferMsg[32] = "Tiempo de uso del procesador: \0";		
	indexFinal = concatenar_array(bufferMsgFinal, bufferMsg, indexFinal);
	int aux = convesor_entero_char_3(tTotalProcesador, bufferMsg, 0);
	indexFinal = concatenar_array(bufferMsgFinal, bufferMsg, indexFinal);
	uint8_t bufferAux[6] = " us\n\0";								//se introduce las unidades, el salto de linea y el caracter de fin de buffer
	indexFinal = concatenar_array(bufferMsgFinal,bufferAux,indexFinal);
															
}

//introduce en el buffer el tiempo total y medio de computo de conecta_k_hay_linea y actualiza el nuevo indice del buffer 
void mostrar_tiempo_hay_linea(){
	uint8_t bufferMsg[100] = "Tiempo de computo de conecta_k_hay_linea\nTotal: \0";
	indexFinal = concatenar_array(bufferMsgFinal, bufferMsg, indexFinal);
	int aux = convesor_entero_char_3(sumHayLinea, bufferMsg, 0);
	indexFinal = concatenar_array(bufferMsgFinal, bufferMsg, indexFinal);
	uint8_t bufferAux[100] = "  Media: \0";
	indexFinal = concatenar_array(bufferMsgFinal, bufferAux, indexFinal);
	int aux2 = convesor_entero_char_3(sumHayLinea/numHayLinea, bufferMsg, 0);
	bufferMsg[aux2] = '\0';								//se introduce el caracter de fin de buffer
	indexFinal = concatenar_array(bufferMsgFinal,bufferMsg,indexFinal);

}

//introduce en el buffer el tiempo total y medio que tardan los jugadores en introucir una jugada y actualiza el nuevo indice del buffer 
void mostrar_tiempo_humano(){
	
	uint8_t bufferMsg[100] = "\nTiempo del humano en pensar\nTotal: \0";
	indexFinal = concatenar_array(bufferMsgFinal, bufferMsg, indexFinal);
	int aux = convesor_entero_char_3(sumHumano, bufferMsg, 0);
	indexFinal = concatenar_array(bufferMsgFinal, bufferMsg, indexFinal);
	uint8_t bufferAux[100] = "  Media: \0";
	indexFinal = concatenar_array(bufferMsgFinal, bufferAux, indexFinal);
	int aux2 = convesor_entero_char_3(sumHumano/numHumano, bufferMsg, 0);
	bufferMsg[aux2] = '\0';								//se introduce el caracter de fin de buffer
	indexFinal = concatenar_array(bufferMsgFinal,bufferMsg,indexFinal);
}


//muestra el numero total de eventos encolados y el numero de cada tipo de evento de cada partida y devuelve el nuevo indice del buffer 
void mostrar_estadisticas(){
	
	//array con los tipos de eventos que se van a mostrar por pantalla
	uint8_t bufferMensajes[NUMEVENTOS][25] = {"\nTOTAL: \0","\nTIMER: \0", "\nALARMA_OVERFLOW: \0", "\nBOTON: \0", "\nBOTON_EINT1_ALARM: \0", "\nBOTON_EINT2_ALARM: \0", "\nDEEP_SLEEP: \0", "\nev_LATIDO: \0", "\nev_VISUALIZAR_HELLO: \0", 
	 "\nev_RX_SERIE: \0", "\nev_TX_SERIE: \0",  "\nev_JUEGO: \0"};
	
	//int index = 0;
	uint8_t bufferEstadistica[100];
	
	uint32_t total_eventos = callback_fifo_estadisticas(VOID);
	indexFinal = concatenar_array(bufferMsgFinal, bufferMensajes[0], indexFinal);
	convesor_entero_char_3((callback_fifo_estadisticas(0)), bufferEstadistica, 0); 
	indexFinal = concatenar_array(bufferMsgFinal, bufferEstadistica, indexFinal);
	
	//se recorre el array de mensajes y se introduce en el buffer el numero de cada tipo de evento
	for(int i=1; i<NUMEVENTOS; i++){
		indexFinal = concatenar_array(bufferMsgFinal, bufferMensajes[i], indexFinal);
		convesor_entero_char_3((callback_fifo_estadisticas(i)), bufferEstadistica, 0); 
		indexFinal = concatenar_array(bufferMsgFinal, bufferEstadistica, indexFinal);
		
	}
	bufferMsgFinal[indexFinal] = '\n';
	//bufferMsg[index+1] = '%';
	//int ind = concatenar_array(buffer,bufferMsg,0);
	indexFinal += 1;
}



//introduce en el buffer el mensaje con las indicaciones para empezar una nueva partida y devuelve el nuevo indice del buffer 
void mostrar_volver_a_jugar(){
	uint8_t bufferMsg[120] = "****************************\nINTRODUCE EL COMANDO $NEW! PARA VOLVER A JUGAR\n****************************\n\n\0";
	indexFinal = concatenar_array(bufferMsgFinal,bufferMsg,indexFinal);
	bufferMsgFinal[indexFinal] = '\0';									//se introduce el caracter de fin de buffer
	indexFinal +=  1;										//devuelve el nuevo indice del buffer

	
}

//muestra por linea serie el mensaje de movimiento cancelado por un jugador
void mostrar_movimiento_cancelado(){
	uint8_t bufferMsg[22] = "MOVIMIENTO CANCELADO\n\0";
	linea_serie_drv_enviar_array(bufferMsg);
}


//muestra por linea serie el mensaje de error correspondiente,  0 -> comando erroneo, 1 -> fila-columna invalida
void mostrar_error_juego(int razon){ 
	//depende del tipo de error se muestra un mensaje u otro
	if(razon == 0){
		uint8_t bufferMsg[100] = "*****************************\nCOMANDO ERRONEO\n*****************************\n\n\0";
		linea_serie_drv_enviar_array(bufferMsg);
	}else if(razon == 1){
		uint8_t bufferMsg[100] = "*****************************\nCOLUMNA INVALIDA\n*****************************\n\n\0";
		linea_serie_drv_enviar_array(bufferMsg);
	}
	
}

// funcion que muestra la pantalla final en la cual se encuentra la causa, tiempo total de uso del procesador, tiempo
// total y medio en computo de conecta_k_hay_linea, tiempo total y media de tiempo que al humano le cuesta pensar la jugada 
// y el total de eventos encolados en la cola de eventos.
void mostrar_pantalla_final_juego(){ //0 -> victoria, 1 -> cancel, 2 -> end
	
	//uint8_t bufferMsgAux[300];
	indexFinal = 0;
	//introducimos en el bufferMsgFinal el texto a imprimir en pantalla
	mostrar_titulo_final_juego();
	mostrar_causa();
	mostrar_tiempo_procesador();
	mostrar_tiempo_hay_linea();
	mostrar_tiempo_humano();
	mostrar_estadisticas();
	mostrar_volver_a_jugar();
	bufferMsgFinal[indexFinal] = '\0';						//se coloca el caracter de final de buffer
	linea_serie_drv_enviar_array(bufferMsgFinal);

}


//funcion que reinicia la matriz "salida"
void limpiar_salida(){
	//recorre toda la matriz y pone cada componente a 0
	for(int i=0; i<7; i++){
			for(int j=0; j<7; j++){
				salida[i][j] = 0;
			}
		}
	
}

//funcion que reinicia las estadisticas de la cola fifo
void reiniciar_estadisticas(){
	callback_fifo_reiniciar_estadisticas(); 
}


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES PUBLICAS------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//comienzo del juego, inicializa el tablero y muestra mensaje inicial por pantalla
void juego_inicializar(void (*callback_gpio_hal_sentido_param)(), void (*callback_gpio_hal_escribir_param)(), uint32_t (*callback_gpio_hal_leer_param)(), int GPIO_JUGAR_ERROR_PARAM, int GPIO_JUGAR_ERROR_BITS_PARAM, int GPIO_HAL_PIN_DIR_OUTPUT_PARAM, uint32_t (*callback_fifo_estadisticas_param)(), void (*callback_fifo_reiniciar_estadisticas_param)()){
	cuenta = 0;											//inicializamos las variables
	intervalo = 0;
	GPIO_JUGAR_ERROR = GPIO_JUGAR_ERROR_PARAM;			//asignamos a cada variable el valor correspondiente pasado por parametros
	GPIO_JUGAR_ERROR_BITS = GPIO_JUGAR_ERROR_BITS_PARAM;
	callback_gpio_hal_sentido_param(GPIO_JUGAR_ERROR_PARAM, GPIO_JUGAR_ERROR_BITS_PARAM, GPIO_HAL_PIN_DIR_OUTPUT);
	callback_gpio_hal_escribir = callback_gpio_hal_escribir_param;
	callback_gpio_hal_leer = callback_gpio_hal_leer_param;
	callback_fifo_estadisticas = callback_fifo_estadisticas_param;
	callback_fifo_reiniciar_estadisticas = callback_fifo_reiniciar_estadisticas_param;
	tablero_inicializar(&cuadricula);					//inicalizamos el tablero
	uint8_t bufferMsgIni[] = "****************************\n\tCONECTA K\nPULSE UN BOTON PARA INICIAR\nO ESCRIBA EL COMANDO $NEW!\nPARA REALIZAR UNA JUGADA\nDEBE INTRODUCIR EL COMANDO ($#-#!)\nPARA FINALIZAR LA PARTIDA\nPULSE EL BOTON 2 O\nINTRODUZCA EL COMANDO $END!\n****************************\n\n\0";
	state = ESCRITURA_PAG_PRINCIPAL;
	linea_serie_drv_enviar_array(bufferMsgIni);			

}





//maquina de estados de nuestro juego
void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData){
	if (state == ESCRITURA_PAG_PRINCIPAL){						//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 							// evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_INICIO_PARTIDA;
		}
		
	}else if (state == WAIT_INICIO_PARTIDA ){					//estado de espera a que un jugador  comience la partida
		if (ID_evento == ev_RX_SERIE){ 							//evento de comando introucido por linea serie
			uint8_t bufferTratarEvento[5]; 						//buffer en el cual se introduce el comando para comprbar si es "NEW"
			bufferTratarEvento[0] = (auxData >> 16 ) & 0xFF; 	//primer caracter
			bufferTratarEvento[1] = (auxData >> 8) & 0xFF;		//segundo caracter
			bufferTratarEvento[2] = (auxData) & 0xFF;			//tercero caracter
			bufferTratarEvento[3] = '\n';						//caracter salto de linea
			bufferTratarEvento[4] = '\0';						//caracter que utilizamos como fin de buffer
			
			if((bufferTratarEvento[0] == 'N' && bufferTratarEvento[1] == 'E'&& bufferTratarEvento[2] == 'W')){
				//comienza una nueva partida
				turno = turnoEmpieza;							//se establce el turno correspondiente
				if(turnoEmpieza == 1){ 							//se cambia el turno para la siguiente partida 
					turnoEmpieza = 2;
				}else{
					turnoEmpieza = 1;
				}
				reiniciar_estadisticas();						//reiniciamos las estadisticas para que correspondan a la partida actual
				t1Procesador = clock_get_us();					//momento en el que comieza la partida
				limpiar_tablero(&cuadricula);					//se vacia el tablero
				limpiar_salida();								//se vacia la salida del tablero
				conecta_K_test_cargar_tablero(&cuadricula);		//se carga el tablero test (opcional)
				conecta_K_visualizar_tablero_juego();			//se muestra el tablero inicial y el turno
				state = ESCRITURA_MOSTRAR_TABLERO;
			}
		}else if(ID_evento == BOTON){							//evento de pulsacion de un boton
			if (primeraVez == 0){								//se mira si es la primera vez que se pulsa un boton (practica 2b)
				primeraVez = 1;
			}else{
				uint64_t tiempo_actual;							//se obtiene el tiempo actual (practica 2b)
				tiempo_actual = clock_get_us();
				intervalo = tiempo_actual - ultimaPulsacion;	//se calcula el tiempo desde la ultima pulsacion (practica 2b)
				ultimaPulsacion = tiempo_actual;
			}
			turno = turnoEmpieza;								//se establce el turno correspondiente
			if(turnoEmpieza == 1){	 							//se cambia el turno para la siguiente partida 
				turnoEmpieza = 2;
			}else{
				turnoEmpieza = 1;
			}
			reiniciar_estadisticas(); 							//reiniciamos las estadisticas para que correspondan a la partida actual
			t1Procesador = clock_get_us();						//momento en el que comieza la partida
			limpiar_tablero(&cuadricula);						//se vacia el tablero
			limpiar_salida();									//se vacia la salida del tablero
			conecta_K_test_cargar_tablero(&cuadricula);			//se carga el tablero test (opcional)
			conecta_K_visualizar_tablero_juego();				//se muestra el tablero inicial y el turno
			state = ESCRITURA_MOSTRAR_TABLERO;
		}
	}else if (state == ESCRITURA_MOSTRAR_TABLERO){ 				//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 							//evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_COMANDO;
			t1Humano = clock_get_us();							//se obtiene el tiempo actual
		}

	}else if (state == WAIT_COMANDO){ 							//estado de espera a que un jugador introduzca una jugada o finalice la partida
		t2Humano = clock_get_us();								//momento en el que introuce la jugada
		numHumano++;											//aumenta en una unidad el numero de jugadas introducidas
		sumHumano += t2Humano-t1Humano;							//se calcula la diferencia entre el tiempo inicial de la espera y el tiempo en que se introduce la jugada
		if (ID_evento == ev_RX_SERIE){ 							//evento de comando introucido por linea serie
			uint8_t bufferTratarEvento[5];						//buffer en el cual se introduce el comando para comprbar si es "END" o una jugada
			bufferTratarEvento[0] = (auxData >> 16 ) & 0xFF;	//primer caracter
			bufferTratarEvento[1] = (auxData >> 8) & 0xFF;		//segundo caracter
			bufferTratarEvento[2] = (auxData) & 0xFF;			//tercer caracter
			bufferTratarEvento[3] = '\n';						//caracter salto de linea
			bufferTratarEvento[4] = '\0';						//caracter que utilizamos como fin de buffer

			//si el comando es "END" se finaliza la partida y se muestra por pantalla la pantalla final
			if((bufferTratarEvento[0] == 'E' && bufferTratarEvento[1] == 'N'&& bufferTratarEvento[2] == 'D')){
				//leemos el estado del pin de error
				if(callback_gpio_hal_leer(GPIO_JUGAR_ERROR,GPIO_JUGAR_ERROR_BITS) == 1){
					//si el bit de error esta activo se desactiva
					callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0); 
				}
				reason = 2;										//se establece el motivo del final de partida
				t2Procesador = clock_get_us();					//se obtiene el momento del final de la partida
				mostrar_pantalla_final_juego();					//se muestra la pantalla final
				state = ESCRITURA_MOSTRAR_FIN;

			//si el comadando es una jugada valida se comprueba que se puede colocar en el tablero y se realiza el movimiento
			}else if((	bufferTratarEvento[0] >= '1' && bufferTratarEvento[0] <= '7' 
						&& bufferTratarEvento[1] == '-' 
						&& bufferTratarEvento[2] >= '1' && bufferTratarEvento[2] <= '7'))
			{		
				fila = bufferTratarEvento[0] - '0'; 			//se obtiene la fila
				columna = bufferTratarEvento[2] - '0';			// se obtiene la columna
				
				//se comprueba si la celda esta vacia, es decir, se puede colocar la ficha
				if (celda_vacia(tablero_leer_celda(&cuadricula, fila -1, columna -1))){
					//leemos el estado del pin de error
					if(callback_gpio_hal_leer(GPIO_JUGAR_ERROR,GPIO_JUGAR_ERROR_BITS) == 1){
						//si el bit de error esta activo se desactiva
						callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0); 
					}
					conecta_K_visualizar_movimiento_juego(); 	//se visualiza el movimiento introducido y el mensaje de cancelacion
					state = ESCRITURA_COMANDO_CORRECTO;
					alarma_activar(ev_JUEGO, 3000, 0); 			//alarma para permitir los tres segundos de cancelacion
				}else{
					//celda ocupada y se muestra el led de error
					callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 1);
					mostrar_error_juego(1); 					//se muestra el mensaje de error y su causa
					state = ESCRITURA_MOSTRAR_ERROR;
				}
			
			}else{
				//comando introducido erroneo y se muestra el led de erroneo
				callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 1);
				mostrar_error_juego(0); 						//se muestra el mensaje de error y su causa
				state = ESCRITURA_MOSTRAR_ERROR;
			}
		}else if(ID_evento == BOTON && auxData == 2){ 			//evento de boton 2 pulsado
			//leemos el estado del pin de error
			if(callback_gpio_hal_leer(GPIO_JUGAR_ERROR,GPIO_JUGAR_ERROR_BITS) == 1){
				//si el bit de error esta activo se desactiva
				callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0); 
			}
			uint64_t tiempo_actual;								//se obtiene el tiempo actual (practica 2b)
			tiempo_actual = clock_get_us();
			intervalo = tiempo_actual - ultimaPulsacion; 		//se calcula el tiempo desde la ultima pulsacion (practica 2b)
			ultimaPulsacion = tiempo_actual; 
			reason = 1; 										//se establece el motivo del final de partida
			t2Procesador = clock_get_us();						//se obtiene el momento del final de la partida
			mostrar_pantalla_final_juego();						//se muestra la pantalla final
			state = ESCRITURA_MOSTRAR_FIN;

		}
	}else if (state == ESCRITURA_COMANDO_CORRECTO){ 			//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 							// evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_CANCELAR;
		}
	}else if (state == WAIT_CANCELAR){
		if(ID_evento == ev_JUEGO){ 								//evento de jugada valida y no cancelada
			//se inserta la ficha en el tablero
			if(tablero_insertar_color(&cuadricula, fila-1, columna-1, turno) == EXITO) {
					t1HayLinea = clock_get_us();				//se obtiene el tiempo previo a ejecutar conecta_k_hay_linea
					//se comprueba si ha ganado un jugador
					uint8_t hayLinea = conecta_K_verificar_K_en_linea(&cuadricula, fila-1, columna-1, turno);
					t2HayLinea = clock_get_us();				//se obtiene el tiempo tras ejecutar conecta_k_hay_linea
					sumHayLinea += t2HayLinea - t1HayLinea;		//se calcula el tiempo de computacion de conecta_k_hay_linea
					numHayLinea++;								//aumenta en una unidad el numero de llamadas a conecta_k_hay_linea
					//si hay victoria se acaba la partida
					if(hayLinea){
						state = ESCRITURA_TABLERO_FIN;
						t2Procesador = clock_get_us(); 			//se obtiene el tiempo final de la partida
						conecta_K_visualizar_tablero_ganador();	//se muestra la pantalla final
						reason = 0;								//se establece la razon de final de partida por victoria
					}else{
						//si no hay victoria se cambia el turno 
						if (turno == 1){
							turno = 2;
						}else{
							turno = 1;
						}
						state = ESCRITURA_MOSTRAR_TABLERO;
						conecta_K_visualizar_tablero_juego();	//se muestra el tablero con la nueva jugada y el turno correspondiente
					}
			}
			
		}else if(ID_evento == BOTON && auxData == 1){			//evento de boton 22 pulsado
			alarma_activar(ev_JUEGO, 0, 0);						//se desactiva la alarma de cancelacion
			mostrar_movimiento_cancelado();						//me muestra mensaje de movimiento cancelado
			state = CANCELADO;
		}
	}else if ( state == CANCELADO){								//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){							// evento que indica finalización de la escritura y se avanza al siguiente estado
			conecta_K_visualizar_tablero_juego();				//se muestra el tablero sin la jugada cancelada
			state = WAIT_COMANDO;
			t1Humano = clock_get_us();							//se obtiene el tiempo actual
		}
	}else if (state == ESCRITURA_TABLERO_FIN){ 					//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 							// evento que indica finalización de la escritura y se avanza al siguiente estado
			mostrar_pantalla_final_juego();
			state = ESCRITURA_MOSTRAR_FIN;
		}
	}else if ( state == ESCRITURA_MOSTRAR_FIN){ 				//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 							// evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_INICIO_PARTIDA;
		}
	}else if(state == ESCRITURA_MOSTRAR_ERROR){ 				//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 							// evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_COMANDO;
		}
	}

	
}

