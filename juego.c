
#include "juego.h"

static volatile int cuenta;						//veces que se han pulsado los botones (practica 2b)
static volatile int intervalo;					//tiempo (practica 2b)
static volatile int ultimaPulsacion = 0;		//momento en el que se pulsa por última vez(practica 2b)
static volatile int primeraVez = 0;				//booleano que indica si es la primera vez que se pulsa un boton (practica 2b)
static int GPIO_JUGAR_ERROR;					//valor del pin de error jugada incorrecta
static int GPIO_JUGAR_ERROR_BITS;				//valor del tamaño en bits que corresponden al error 
static void (*callback_gpio_hal_escribir)();	//puntero a la funcion gpio_hal_escribir
static uint8_t salida[8][8];					//salida del tablero
static TABLERO cuadricula;						//tablero de juego
static uint32_t t1;								//tiempo previo a mostrar el tablero (demostrador practica 2c)
static uint8_t turno = 1;						//indica a que jugador le toca mover ficha
static uint32_t turnoEmpieza = 1; 				//numero del jugador que empieza la siguiente partida
static uint8_t fila = 0;						//valor de la fila introducida por un jugador
static uint8_t columna = 0;						//valor de la columna introducida por un jugador
static uint8_t reason = 0; 						//causa del final de la partida: 0 -> victoria, 1 -> cancelada mediante boton, 2 -> comando end introducido

//variables para calcular el tiempo del procesador en una partida
static uint32_t t1Procesador; 					// tiempo de comienzo de la partida
static uint32_t t2Procesador; 					// tiempo de final de partida

//variables para calcular el tiempo de computo de la funcion conecta_k_hay_linea
static uint32_t t1HayLinea; 					//tiempo previo a ejecutar conecta_k_hay_linea
static uint32_t t2HayLinea; 					//tiempo tras ejecutar conecta_k_hay_linea
static uint32_t sumHayLinea = 0;				//tiempo total  que se tarda en ejecutar conecta_k_hay_linea
static uint32_t numHayLinea = 0; 				//numero de veces que se ejecuta conecta_k_hay_linea

//variables para calcular el tiempo que les cuesta a los jugadoresa introducir una jugada
static uint32_t t1Humano;						//tiempo desde que el usuario puede introducir un comando
static uint32_t t2Humano;						//tiempo en el que el usuario introduce un comando  
static uint32_t sumHumano = 0; 					//tiempo total  que tardan los jugadores en introducir un comando
static uint32_t numHumano = 0; 					//numero de veces que los jugadores introducen un comando





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

static  state = PAG_PRINCIPAL;


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES PRIVADAS------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE AUXILIARES ------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//calcula el tiempo que tarda en mostrar el tablero (practica 2c)
void tiempo_visualizar_tablero(uint32_t t2){

	uint32_t t3;
	t3 = t2-t1;
	conecta_K_visualizar_tiempo(t3);
}

int concatenar_array(uint8_t buffer1[], uint8_t buffer2[], int index){
	int j=0;
	while(buffer2[j] != '%'){
		buffer1[index+j] = buffer2[j];
		j++;
	}
	return index+j;
}




//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie, al final del buffer se añade los caracteres '\n' y '%'
void convesor_entero_char(uint32_t num, uint8_t array_digitos[]){
	int numAux = num;
	unsigned int longitud = 0;
	while (numAux != 0) {
        numAux /= 10;
        longitud++;
    }
    // Crear un array para almacenar los dígitos
    numAux = num;
	array_digitos[longitud] = '\n';
	array_digitos[longitud+1] = '%';
    // Separar cada dígito y almacenarlo en el array
    for (int i = longitud - 1; i >= 0; i--) {
        array_digitos[i] = (numAux % 10) + '0';
        numAux /= 10;
    }
	
}

//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie, al final del buffer no se añade los caracteres '\n' y '%'
int convesor_entero_char_2(uint32_t num, uint8_t array_digitos[], int indice){
	int numAux = num;
	unsigned int longitud = 0;
	while (numAux != 0) {
        numAux /= 10;
        longitud++;
    }
    // Crear un array para almacenar los dígitos
    numAux = num;

    // Separar cada dígito y almacenarlo en el array
    for (int i = longitud - 1 + indice; i >= indice; i--) {
        array_digitos[i] = (numAux % 10) + '0';
        numAux /= 10;
    }
	
	return longitud;
}

//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie, al final del buffer se añade el caracterer '%' 
//y si num = 0 devuelve el caracter 0
int convesor_entero_char_3(uint32_t num, uint8_t array_digitos[], int indice){
	int numAux = num;
	unsigned int longitud = 0;
	if(num == 0){
		longitud = 1;
	}else{
		while (numAux != 0) {
        	numAux /= 10;
        	longitud++;
    	}
	}

    // Crear un array para almacenar los dígitos
    numAux = num;

    // Separar cada dígito y almacenarlo en el array
    for (int i = longitud - 1 + indice; i >= indice; i--) {
        array_digitos[i] = (numAux % 10) + '0';
        numAux /= 10;
    }
	array_digitos[longitud] = '%';
	return longitud+1;
}

//muestra el tiempo que tarda en mostrar el tablero, el cual ha sido previamente calculado
void conecta_K_visualizar_tiempo(uint32_t num){
	uint8_t array_digitos[100];
	convesor_entero_char(num,array_digitos);
	linea_serie_drv_enviar_array(array_digitos);
}






//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE VISUALIZACION TABLERO--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//convierte la matriz salida que hay en memoria en un vector y devuelve la posición siguiente al último elemento del vector
int tablero_to_array(uint8_t bufferTablero[]){
	int i=0;
	int j=0;
	int aux = 0;

	while(i < 8){
		j=0;
		while(j<8 || j==8 && aux == 1){
			if(i==0){
				if(aux == 0){
					bufferTablero[j*2] = '0' + j;
					aux=1;
				}else{
					bufferTablero[j*2+aux] = '|';
					aux=0;
					j++;
				}
			}else if(j==0){
				if(aux % 2 == 0){
					bufferTablero[i*8*2+j*2+i] = '0' + i;
					aux=1;
				}else{
					bufferTablero[i*8*2+j*2+aux+i] = '|';
					aux=0;
					j++;
				}	
			}else{
				if(aux == 0){
					if(salida[i][j] == 0x11){
						bufferTablero[i*8*2+j*2+i] = 'B';
					}else if(salida[i][j] == 0x22){
						bufferTablero[i*8*2+j*2+i] = 'N';
					}else{
						bufferTablero[i*8*2+j*2+i] = ' ';
					}	
					aux=1;	
				}else{
					bufferTablero[i*8*2+j*2+aux+i] = '|';
					aux=0;
					j++;
				}
			}
		}
		bufferTablero[i*8*2+j*2+i] = '\n';
		i++;
	}
	return i*8*2+i;
}




//muestra el tablero final y muestra el número del jugador que ha ganado
void conecta_K_visualizar_tablero_ganador(){
	uint8_t bufferTablero[300];
	t1 = clock_get_us();
	//se visualiza el tablero
	conecta_K_visualizar_tablero(&cuadricula, salida);
	
	int ind = tablero_to_array(bufferTablero);
	uint8_t bufferAux[42] = "Ganador:  \n***************************\n\n%";
	bufferAux[9] = turno +'0';
	
	ind = concatenar_array(bufferTablero, bufferAux, ind);

	bufferTablero[ind] = '%';
	linea_serie_drv_enviar_array(bufferTablero);
}



//muestra el estado actual del tablereo y el turno del jugador que le corresponde
void conecta_K_visualizar_tablero_juego(){
	uint8_t bufferTablero[300];
	t1 = clock_get_us();
	//se visualiza el tablero
	conecta_K_visualizar_tablero(&cuadricula, salida);
	
	int ind = tablero_to_array(bufferTablero);
	uint8_t bufferAux[40] = "Turno:  \n***************************\n\n%";
	bufferAux[7] = turno +'0';
	ind = concatenar_array(bufferTablero, bufferAux, ind);
	bufferTablero[ind] = '%';
	linea_serie_drv_enviar_array(bufferTablero);
}


//muestra el movimiento introducido por un jugador y el mensaje informando sobre la cancelacion de este
void conecta_K_visualizar_movimiento_juego(){// se puede llamar a una funcion nueva que sea parecida a conecta k visualizar tablero juego

	uint8_t bufferTablero[300];

	conecta_K_visualizar_tablero(&cuadricula, salida);

	int ind = tablero_to_array(bufferTablero);
	if(turno==1){
		bufferTablero[fila*8*2+columna*2+fila] = 'B';
	}else if(turno ==2){
		bufferTablero[fila*8*2+columna*2+fila] = 'N' ;
	}
	uint8_t bufferAux[65] = "PULSA EL BOTON 1 PARA CANCELAR\n***************************\n\n%";
	ind = concatenar_array(bufferTablero, bufferAux, ind);
	bufferTablero[ind] = '%';

	linea_serie_drv_enviar_array(bufferTablero);

}




//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE VISUALIZACION TEXTO--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//introduce en el buffer el titulo de partida fin
int mostrar_titulo_final_juego(uint8_t buffer[], int index){ //0 -> victoria, 1 -> cancel, 2 -> end
	int ind;
	if (reason == 1 || reason == 2){
		uint8_t bufferMsgFin[38] = "FIN DE LA PARTIDA\nPARTIDA CANCELADA\n%";
		ind = concatenar_array(buffer,bufferMsgFin,index);
	}else if( reason == 0){
		uint8_t bufferMsgFin[39] = "FIN DE LA PARTIDA\nPARTIDA FINALIZADA\n%";
		ind = concatenar_array(buffer,bufferMsgFin,index);

	}
	return ind;
	
}

//introduce en el buffer el motivo de finalización de partida
int mostrar_causa(uint8_t buffer[], int index){
	int ind;
	if(reason == 0){
		uint8_t bufferMsg[32] = "CAUSA: VICTORIA DEL JUGADOR  \n%";
		bufferMsg[28] = turno +'0';
		ind = concatenar_array(buffer,bufferMsg,index);

	}else if(reason == 1){
		uint8_t bufferMsg[25] = "CAUSA: BOTON 2 PULSADO\n%";
		ind = concatenar_array(buffer,bufferMsg,index);

	}else if(reason == 2) {
		uint8_t bufferMsg[33] = "CAUSA: COMANDO END INTRODUCIDO\n%";
		ind = concatenar_array(buffer,bufferMsg,index);
	
	}

	return ind;

	
}

//introduce en el buffer el tiempo total del uso del procesador 
int mostrar_tiempo_procesador(uint8_t buffer[], int index){
	uint8_t bufferMsg[100] = "Tiempo de uso del procesador: ";
	uint32_t tTotalProcesador = t2Procesador -t1Procesador;
	int aux = convesor_entero_char_2(tTotalProcesador, bufferMsg, 31);

	bufferMsg[31+aux] = 'u';
	bufferMsg[31+aux+1] = 's';
	bufferMsg[31+aux+2] = '\n';
	bufferMsg[31+aux+3] = '%';
	int ind = concatenar_array(buffer,bufferMsg,index);
	return ind;
}

//introduce en el buffer el tiempo total y medio de computo de conecta_k_hay_linea
int mostrar_tiempo_hay_linea(uint8_t buffer[], int index){
	uint8_t bufferMsg[100] = "Tiempo de computo de conecta_k_hay_linea\nTotal:";
	int aux = convesor_entero_char_2(sumHayLinea, bufferMsg, 48);
	bufferMsg[48+aux] = ' ';
	bufferMsg[48+aux+1] = 'M';
	bufferMsg[48+aux+2] = 'e';
	bufferMsg[48+aux+3] = 'd';
	bufferMsg[48+aux+4] = 'i';
	bufferMsg[48+aux+5] = 'a';
	bufferMsg[48+aux+6] = ':';
	bufferMsg[48+aux+7] = ' ';
	int aux2 = convesor_entero_char_2(sumHayLinea/numHayLinea, bufferMsg, 48+aux+8);
	bufferMsg[48+aux+8+aux2] = '\n';
	bufferMsg[48+aux+8+aux2+1] = '%';

	int ind = concatenar_array(buffer,bufferMsg,index);
	return ind;

}

//introduce en el buffer el tiempo total y medio que tardan los jugadores en introucir una jugada
int mostrar_tiempo_humano(uint8_t buffer[], int index){
	// char bufferMsg[3]="h\n%";
	// linea_serie_drv_enviar_array(bufferMsg);
	uint8_t bufferMsg[100] = "Tiempo del humano en pensar\nTotal:";
	int aux = convesor_entero_char_2(sumHumano, bufferMsg, 35);
	bufferMsg[35+aux] = ' ';
	bufferMsg[35+aux+1] = 'M';
	bufferMsg[35+aux+2] = 'e';
	bufferMsg[35+aux+3] = 'd';
	bufferMsg[35+aux+4] = 'i';
	bufferMsg[35+aux+5] = 'a';
	bufferMsg[35+aux+6] = ':';
	bufferMsg[35+aux+7] = ' ';
	int aux2 = convesor_entero_char_2(sumHumano/numHumano, bufferMsg, 35+aux+8);
	bufferMsg[35+aux+8+aux2] = '\n';
	bufferMsg[35+aux+8+aux2+1] = '%';

	int ind = concatenar_array(buffer,bufferMsg,index);
	return ind;
}


//muestra el numero total de eventos encolados y el numero de cada tipo de evento de cada partida
int mostrar_estadisticas(uint8_t buffer[], int index2){
	
	uint8_t bufferMensajes[NUMEVENTOS][25] = {"TOTAL: %","\nTIMER: %", "\nALARMA_OVERFLOW: %", "\nBOTON: %", "\nBOTON_EINT1_ALARM: %", "\nBOTON_EINT2_ALARM: %", "\nDEEP_SLEEP: %", "\nev_LATIDO: %", "\nev_VISUALIZAR_HELLO: %", 
	 "\nev_RX_SERIE: %", "\nev_TX_SERIE: %",  "\nev_JUEGO: %"};
	//uint8_t bufferMsg[300];
	int index = 0;
	uint8_t bufferEstadistica[100];
	
	uint32_t total_eventos = FIFO_estadisticas(VOID);
	index = concatenar_array(buffer, bufferMensajes[0], index2);
	convesor_entero_char_3((FIFO_estadisticas(0)), bufferEstadistica, 0); //pasar por parametro la funcion fifo_estadisticas (callback)
	index = concatenar_array(buffer, bufferEstadistica, index);
		
	for(int i=1; i<NUMEVENTOS; i++){
		index = concatenar_array(buffer, bufferMensajes[i], index);
		convesor_entero_char_3((FIFO_estadisticas(i)), bufferEstadistica, 0); ///pasar por parametro la funcion fifo_estadisticas (callback)
		index = concatenar_array(buffer, bufferEstadistica, index);
		
	}
	buffer[index] = '\n';
	//bufferMsg[index+1] = '%';
	//int ind = concatenar_array(buffer,bufferMsg,0);
	return index+1;
}



//introduce en el buffer el mensaje con las indicaciones para empezar una nueva partida
int mostrar_volver_a_jugar(uint8_t buffer[], int index){
	uint8_t bufferMsg[120] = "****************************\nINTRODUCE EL COMANDO $NEW! PARA VOLVER A JUGAR\n****************************\n\n%";
	int ind = concatenar_array(buffer,bufferMsg,index);
	buffer[ind] = '%';
	return ind+1;

	
}

//muestra por linea serie el mensaje de movimiento cancelado por un jugador
void mostrar_movimiento_cancelado(){
	uint8_t bufferMsg[22] = "MOVIMIENTO CANCELADO\n%";
	linea_serie_drv_enviar_array(bufferMsg);
}


//muestra por linea serie el mensaje de error correspondiente
void mostrar_error_juego(int razon){ // 0-> comando erroneo, 1-> fila-columna invalida
	if(razon == 0){
		uint8_t bufferMsg[100] = "*****************************\nCOMANDO ERRONEO\n*****************************\n\n%";
		linea_serie_drv_enviar_array(bufferMsg);
	}else if(razon == 1){
		uint8_t bufferMsg[100] = "*****************************\nCOLUMNA INVALIDA\n*****************************\n\n%";
		linea_serie_drv_enviar_array(bufferMsg);
	}
	
}

// funcion que muestra la pantalla final en la cual se encuentra la causa, tiempo total de uso del procesador, tiempo
// total y medio en computo de conecta_k_hay_linea, tiempo total y media de tiempo que al humano le cuesta pensar la jugada 
// y el total de eventos encolados en la cola de eventos.
void mostrar_pantalla_final_juego(){ //0 -> victoria, 1 -> cancel, 2 -> end
	uint8_t bufferMsgFinal[600];
	uint8_t bufferMsgAux[300];
	int index = 0;

	index = mostrar_titulo_final_juego(bufferMsgFinal, 0);
	index = mostrar_causa(bufferMsgFinal, index);
	index = mostrar_tiempo_procesador(bufferMsgFinal, index);
	index = mostrar_tiempo_hay_linea(bufferMsgFinal, index);
	index = mostrar_tiempo_humano(bufferMsgFinal, index);
	index = mostrar_estadisticas(bufferMsgFinal, index);
	index = mostrar_volver_a_jugar(bufferMsgFinal, index);
	bufferMsgFinal[index] = '%';
	linea_serie_drv_enviar_array(bufferMsgFinal);

}


//funcion que reinicia la matriz "salida"
void limpiar_salida(){
	for(int i=0; i<7; i++){
			for(int j=0; j<7; j++){
				salida[i][j] = 0;
			}
		}
	
}

//funcion que reinicia las estadisticas de la cola fifo
void reiniciar_estadisticas(){
	FIFO_reiniciar_estadisticas(); // hay que pasarlo por parametros
}


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES PUBLICAS------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//comienzo del juego, inicializa el tablero y muestra mensaje inicial por pantalla
void juego_inicializar(void (*callback_gpio_hal_sentido_param)(), void (*callback_gpio_hal_escribir_param)(), int GPIO_JUGAR_ERROR_PARAM, int GPIO_JUGAR_ERROR_BITS_PARAM, int GPIO_HAL_PIN_DIR_OUTPUT_PARAM){
	cuenta = 0;
	intervalo = 0;
	GPIO_JUGAR_ERROR = GPIO_JUGAR_ERROR_PARAM;
	GPIO_JUGAR_ERROR_BITS = GPIO_JUGAR_ERROR_BITS_PARAM;
	callback_gpio_hal_sentido_param(GPIO_JUGAR_ERROR_PARAM, GPIO_JUGAR_ERROR_BITS_PARAM, GPIO_HAL_PIN_DIR_OUTPUT);
	callback_gpio_hal_escribir = callback_gpio_hal_escribir_param;
	tablero_inicializar(&cuadricula);
	uint8_t bufferMsgIni[] = "****************************\n\tCONECTA K\nPULSE UN BOTON PARA INICIAR\nO ESCRIBA EL COMANDO $NEW!\nPARA REALIZAR UNA JUGADA\nDEBE INTRODUCIR EL COMANDO ($#-#!)\nPARA FINALIZAR LA PARTIDA\nPULSE EL BOTON 2 O\nINTRODUZCA EL COMANDO $END!\n****************************\n\n%";
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
			bufferTratarEvento[4] = '%';						//caracter que utilizamos como fin de buffer
			
			if((bufferTratarEvento[0] == 'N' && bufferTratarEvento[1] == 'E'&& bufferTratarEvento[2] == 'W')){
				//comienza una nueva partida
				reiniciar_estadisticas(); 						//reiniciamos las estadisticas para que correspondan a la partida actual
				turno = turnoEmpieza;							//se establce el turno correspondiente
				if(turnoEmpieza == 1){ 							//se cambia el turno para la siguiente partida 
					turnoEmpieza = 2;
				}else{
					turnoEmpieza = 1;
				}
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
			reiniciar_estadisticas(); 							//reiniciamos las estadisticas para que correspondan a la partida actual
			turno = turnoEmpieza;								//se establce el turno correspondiente
			if(turnoEmpieza == 1){	 							//se cambia el turno para la siguiente partida 
				turnoEmpieza = 2;
			}else{
				turnoEmpieza = 1;
			}
			t1Procesador = clock_get_us();						//momento en el que comieza la partida
			limpiar_tablero(&cuadricula);						//se vacia el tablero
			limpiar_salida();									//se vacia la salida del tablero
			conecta_K_test_cargar_tablero(&cuadricula);			//se carga el tablero test (opcional)
			conecta_K_visualizar_tablero_juego();				//se muestra el tablero inicial y el turno
			state = ESCRITURA_MOSTRAR_TABLERO;
		}
	}else if (state == ESCRITURA_MOSTRAR_TABLERO){ 				//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 							// evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_COMANDO;
			t1Humano = clock_get_us();							//se obtiene el tiempo actual
		}

	}else if (state == WAIT_COMANDO){ 
		t2Humano = clock_get_us();
		numHumano++;
		sumHumano += t2Humano-t1Humano;
		if (ID_evento == ev_RX_SERIE){ 
			uint8_t bufferTratarEvento[5];
			bufferTratarEvento[0] = (auxData >> 16 ) & 0xFF;
			bufferTratarEvento[1] = (auxData >> 8) & 0xFF;
			bufferTratarEvento[2] = (auxData) & 0xFF;
			bufferTratarEvento[3] = '\n';
			bufferTratarEvento[4] = '%';
			if((bufferTratarEvento[0] == 'E' && bufferTratarEvento[1] == 'N'&& bufferTratarEvento[2] == 'D')){
				callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0);
				reason = 2;
				t2Procesador = clock_get_us();
				mostrar_pantalla_final_juego();
				state = ESCRITURA_MOSTRAR_FIN;

			}else if((bufferTratarEvento[0] >= '1' && bufferTratarEvento[0] <= '7' && bufferTratarEvento[1] == '-' && bufferTratarEvento[2] >= '1' && bufferTratarEvento[2] <= '7')){
				fila = bufferTratarEvento[0] - '0';
				columna = bufferTratarEvento[2] - '0';
				if (celda_vacia(tablero_leer_celda(&cuadricula, fila -1, columna -1))){
					callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0);
					conecta_K_visualizar_movimiento_juego();
					state = ESCRITURA_COMANDO_CORRECTO;
					alarma_activar(ev_JUEGO, 3000, 0);
				}else{
					//visualizar error
					callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 1);
					mostrar_error_juego(1);
					state = ESCRITURA_MOSTRAR_ERROR;
				}
			}else{
				callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 1);
				mostrar_error_juego(0);
				state = ESCRITURA_MOSTRAR_ERROR;
			}
		}else if(ID_evento == BOTON && auxData == 2){
			callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0);
			uint64_t tiempo_actual;
			tiempo_actual = clock_get_us();
			intervalo = tiempo_actual - ultimaPulsacion;
			ultimaPulsacion = tiempo_actual;
			reason = 1;
			t2Procesador = clock_get_us();
			mostrar_pantalla_final_juego();
			state = ESCRITURA_MOSTRAR_FIN;

		}
	}else if (state == ESCRITURA_COMANDO_CORRECTO){ //estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 				// evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_CANCELAR;
		}
	}else if (state == WAIT_CANCELAR){
		if(ID_evento == ev_JUEGO){
			if(tablero_insertar_color(&cuadricula, fila-1, columna-1, turno) == EXITO) {
					t1HayLinea = clock_get_us();
					uint8_t hayLinea = conecta_K_verificar_K_en_linea(&cuadricula, fila-1, columna-1, turno);
					t2HayLinea = clock_get_us();
					sumHayLinea += t2HayLinea - t1HayLinea;
					numHayLinea++;
					if(hayLinea){
						state = ESCRITURA_TABLERO_FIN;
						t2Procesador = clock_get_us();
						conecta_K_visualizar_tablero_ganador();
						reason = 0;
					}else{
						if (turno == 1){
							turno = 2;
						}else{
							turno = 1;
						}
						state = ESCRITURA_MOSTRAR_TABLERO;
						conecta_K_visualizar_tablero_juego();		
					}
			}
			
		}else if(ID_evento == BOTON && auxData == 1){
			alarma_activar(ev_JUEGO, 0, 0);
			mostrar_movimiento_cancelado();
			state = CANCELADO;
		}
	}else if ( state == CANCELADO){
		if(ID_evento == ev_TX_SERIE){
			conecta_K_visualizar_tablero_juego();
			state = WAIT_COMANDO;
			t1Humano = clock_get_us();
		}
	}else if (state == ESCRITURA_TABLERO_FIN){ 		//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 				// evento que indica finalización de la escritura y se avanza al siguiente estado
			mostrar_pantalla_final_juego();
			state = ESCRITURA_MOSTRAR_FIN;
		}
	}else if ( state == ESCRITURA_MOSTRAR_FIN){ 	//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 				// evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_INICIO_PARTIDA;
		}
	}else if(state == ESCRITURA_MOSTRAR_ERROR){ 	//estado de espera a que se escriba todo el buffer por linea serie
		if(ID_evento == ev_TX_SERIE){ 				// evento que indica finalización de la escritura y se avanza al siguiente estado
			state = WAIT_COMANDO;
		}
	}

	
}

