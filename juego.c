
#include "juego.h"

static volatile int cuenta;
static volatile int intervalo;
static volatile int ultimaPulsacion = 0;
static volatile int primeraVez = 0;
static void (*callback_fifo_encolar)();
static int GPIO_JUGAR_ERROR;
static int GPIO_JUGAR_ERROR_BITS;
static void (*callback_gpio_hal_escribir)();
static uint8_t salida[8][8];
static TABLERO cuadricula;
static uint32_t t1;
static int last_command_tab = 0; // boolean
static uint8_t turno = 1;
static uint8_t fila = 0;
static uint8_t columna = 0;
static uint8_t reason = 0; //0 -> victoria, 1 -> cancel, 2 -> end
static uint32_t t1Procesador;
static uint32_t t2Procesador;
static uint32_t t1HayLinea;
static uint32_t t2HayLinea;
static uint32_t sumHayLinea = 0;
static uint32_t numHayLinea = 0;
static uint32_t t1Humano;
static uint32_t t2Humano;
static uint32_t sumHumano = 0;
static uint32_t numHumano = 0;
static uint32_t turnoEmpieza = 1; //numero del jugador que empieza la siguiente partida



	
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
	ESCRITURA_MOSTRAR_CAUSA = 13,
	ESCRITURA_MOSTRAR_TIEMPO_PROCESADOR = 14,
	ESCRITURA_MOSTRAR_TIEMPO_HAY_LINEA = 15,
	ESCRITURA_MOSTRAR_TIEMPO_HUMANO = 16,
	ESCRITURA_MOSTRAR_ESTADISTICAS_FIFO = 17,
	ESCRITURA_MOSTRAR_VOLVER_A_JUGAR = 18
	}

static  state = PAG_PRINCIPAL;


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES PRIVADAS------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE VISUALIZACION TABLERO--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------






//muestra el tablero final y muestra el número del jugador que ha ganado
void conecta_K_visualizar_tablero_ganador(){
	uint8_t bufferTablero[300];
	t1 = clock_get_us();
	//se visualiza el tablero
	conecta_K_visualizar_tablero(&cuadricula, salida);
	
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
	bufferTablero[136] = 'G';
	bufferTablero[137] = 'a';
	bufferTablero[138] = 'n';
	bufferTablero[139] = 'a';
	bufferTablero[140] = 'd';
	bufferTablero[141] = 'o';
	bufferTablero[142] = 'r';
	bufferTablero[143] = ':';
	bufferTablero[144] = turno+ '0';

	//.......
	bufferTablero[145] = '\n';
	
	for (int i = 146; i <= 146+28; i++)
	{
		bufferTablero[i]='*';
	}
	bufferTablero[175] = '\n';
	bufferTablero[176] = '\n';
	bufferTablero[177] = '%';
	linea_serie_drv_enviar_array(bufferTablero);
}



//muestra el estado actual del tablereo y el turno del jugador que le corresponde
void conecta_K_visualizar_tablero_juego(){
	uint8_t bufferTablero[300];
	t1 = clock_get_us();
	//se visualiza el tablero
	conecta_K_visualizar_tablero(&cuadricula, salida);
	
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
	bufferTablero[136]='T';
	bufferTablero[137]='u';
	bufferTablero[138]='r';
	bufferTablero[139]='n';
	bufferTablero[140]='o';
	bufferTablero[141]=':';
	bufferTablero[142]='0'+turno;

	//.......
	bufferTablero[143] = '\n';
	
	for (int i = 144; i <= 144+28; i++)
	{
		bufferTablero[i]='*';
	}
	bufferTablero[173] = '\n';
	bufferTablero[174] = '\n';
	bufferTablero[175] = '%';
	linea_serie_drv_enviar_array(bufferTablero);
}


//muestra el movimiento introducido por un jugador y el mensaje informando sobre la cancelacion de este
void conecta_K_visualizar_movimiento_juego(){// se puede llamar a una funcion nueva que sea parecida a conecta k visualizar tablero juego

	uint8_t bufferTablero[300];
	//t1 = clock_get_us();
	//se visualiza el tablero
	conecta_K_visualizar_tablero(&cuadricula, salida);
	
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
	bufferTablero[136] = '\n';
	if(turno==1){
		bufferTablero[fila*8*2+columna*2+fila] = 'B';
	}else if(turno ==2){
		bufferTablero[fila*8*2+columna*2+fila] = 'N' ;
	}
	bufferTablero[137] = 'P';
	bufferTablero[138] = 'U';
	bufferTablero[139] = 'L';
	bufferTablero[140] = 'S';
	bufferTablero[141] = 'A';
	bufferTablero[142] = ' ';
	bufferTablero[143] = 'E';
	bufferTablero[144] = 'L';
	bufferTablero[145] = ' ';
	bufferTablero[146] = 'B';
	bufferTablero[147] = 'O';
	bufferTablero[148] = 'T';
	bufferTablero[149] = 'O';
	bufferTablero[150] = 'N';
	bufferTablero[151] = ' ';
	bufferTablero[152] = '1';
	bufferTablero[153] = ' ';
	bufferTablero[154] = 'P';
	bufferTablero[155] = 'A';
	bufferTablero[156] = 'R';
	bufferTablero[157] = 'A';
	bufferTablero[158] = ' ';
	bufferTablero[159] = 'C';
	bufferTablero[160] = 'A';
	bufferTablero[161] = 'N';
	bufferTablero[162] = 'C';
	bufferTablero[163] = 'E';
	bufferTablero[164] = 'L';
	bufferTablero[165] = 'A';
	bufferTablero[166] = 'R';
	bufferTablero[167] = '\n';

	for (int i = 168; i <= 168+28; i++){
		bufferTablero[i]='*';
	}
	bufferTablero[197] = '\n';
	bufferTablero[198] = '\n';
	bufferTablero[199] = '%';

	linea_serie_drv_enviar_array(bufferTablero);

}


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE AUXILIARES ------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//calcula el tiempo que tarda en mostrar el tablero
void tiempo_visualizar_tablero(uint32_t t2){

	uint32_t t3;
	t3 = t2-t1;
	conecta_K_visualizar_tiempo(t3);
}

int concatenar_array(char buffer1[], char buffer2[], int index){
	int j=0;
	while(buffer2[j] != '%'){
		buffer1[index+j] = buffer2[j];
		j++;
	}
	return index+j;
}




//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie
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

//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie
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

//funcion que dado un entero lo convierte en char para permitir su escritura por linea serie
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
	char array_digitos[300];
	convesor_entero_char(num,array_digitos);
	linea_serie_drv_enviar_array(array_digitos);
}





//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES DE VISUALIZACION TEXTO--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------







//Muestra titulo partida fin
void mostrar_titulo_final_juego(char buffer[]){ //0 -> victoria, 1 -> cancel, 2 -> end
	
	if (reason == 1 || reason == 2){
		char bufferMsgFin[38] = "FIN DE LA PARTIDA\nPARTIDA CANCELADA\n%";
		int ind = concatenar_array(buffer,bufferMsgFin,0);
		buffer[ind] = '%';
	}else if( reason == 0){
		char bufferMsgFin[39] = "FIN DE LA PARTIDA\nPARTIDA FINALIZADA\n%";
		int ind = concatenar_array(buffer,bufferMsgFin,0);
		buffer[ind] = '%';
	}
	
}


void mostrar_causa(char buffer[]){
	if(reason == 0){
		uint8_t bufferMsg[32] = "CAUSA: VICTORIA DEL JUGADOR  \n%";
		bufferMsg[28] = turno +'0';
		int ind = concatenar_array(buffer,bufferMsg,0);
		buffer[ind] = '%';
	}else if(reason == 1){
		uint8_t bufferMsg[25] = "CAUSA: BOTON 2 PULSADO\n%";
		int ind = concatenar_array(buffer,bufferMsg,0);
		buffer[ind] = '%';
	}else if(reason == 2) {
		uint8_t bufferMsg[33] = "CAUSA: COMANDO END INTRODUCIDO\n%";
		int ind = concatenar_array(buffer,bufferMsg,0);
		buffer[ind] = '%';
	}
	
}

void mostrar_tiempo_procesador(char buffer[]){
	uint8_t bufferMsg[100] = "Tiempo de uso del procesador: ";
	uint32_t tTotalProcesador = t2Procesador -t1Procesador;
	int aux = convesor_entero_char_2(tTotalProcesador, bufferMsg, 31);

	bufferMsg[31+aux] = 'u';
	bufferMsg[31+aux+1] = 's';
	bufferMsg[31+aux+2] = '\n';
	bufferMsg[31+aux+3] = '%';
	int ind = concatenar_array(buffer,bufferMsg,0);
	buffer[ind] = '%';
}

void mostrar_tiempo_hay_linea(char buffer[]){
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

	int ind = concatenar_array(buffer,bufferMsg,0);
	buffer[ind] = '%';


}


void mostrar_tiempo_humano(char buffer[]){
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

	int ind = concatenar_array(buffer,bufferMsg,0);
	buffer[ind] = '%';
}


//muestra el numero total de eventos encolados y el numero de cada tipo
void mostrar_estadisticas(char buffer[]){
	
	char bufferMensajes[NUMEVENTOS][25] = {"TOTAL: %","\nTIMER: %", "\nALARMA_OVERFLOW: %", "\nBOTON: %", "\nBOTON_EINT1_ALARM: %", "\nBOTON_EINT2_ALARM: %", "\nDEEP_SLEEP: %", "\nev_LATIDO: %", "\nev_VISUALIZAR_HELLO: %", 
	 "\nev_RX_SERIE: %", "\nev_TX_SERIE: %",  "\nev_JUEGO: %"};
	char bufferMsg[300];
	int index = 0;
	char bufferEstadistica[100];
	
	uint32_t total_eventos = FIFO_estadisticas(VOID);
	index = concatenar_array(bufferMsg, bufferMensajes[0], index);
	convesor_entero_char_3((FIFO_estadisticas(0)), bufferEstadistica, 0); //revisar cuando el valor es 0
	index = concatenar_array(bufferMsg, bufferEstadistica, index);
		
	for(int i=1; i<NUMEVENTOS; i++){
		index = concatenar_array(bufferMsg, bufferMensajes[i], index);
		convesor_entero_char_3((FIFO_estadisticas(i)), bufferEstadistica, 0); //revisar cuando el valor es 0
		index = concatenar_array(bufferMsg, bufferEstadistica, index);
		
	}
	bufferMsg[index] = '\n';
	bufferMsg[index+1] = '%';
	int ind = concatenar_array(buffer,bufferMsg,0);
	buffer[ind] = '%';
}


// //muestra el total de eventos encolados y un histograma con los tipos de eventos encolados, el problema es que la escala no permite que se ven aquellos que tiene pocos eventos
// void mostrar_estadisticas(char buffer[]){
	
// 	char bufferMensajes[NUMEVENTOS][25] = {"TOTAL: %","\nTIMER: %", "\nALARMA_OVERFLOW: %", "\nBOTON: %", "\nBOTON_EINT1_ALARM: %", "\nBOTON_EINT2_ALARM: %", "\nDEEP_SLEEP: %", "\nev_LATIDO: %", "\nev_VISUALIZAR_HELLO: %", 
// 	 "\nev_RX_SERIE: %", "\nev_TX_SERIE: %",  "\nev_JUEGO: %"};
// 	char bufferMsg[300];
// 	int index = 0;
// 	char bufferEstadistica[100];
// 	int longitud = 0;
// 	uint32_t total_eventos = FIFO_estadisticas(VOID);
// 	index = concatenar_array(bufferMsg, bufferMensajes[0], index);
// 	longitud = convesor_entero_char_3(FIFO_estadisticas(0), bufferEstadistica, 0); //revisar cuando el valor es 0
//  	index = concatenar_array(bufferMsg, bufferEstadistica, index);
	
// 	for(int i=1; i<NUMEVENTOS; i++){
// 		index = concatenar_array(bufferMsg, bufferMensajes[i], index);
// 		int max = (FIFO_estadisticas(i)*50/total_eventos);
// 		int j = 0;
// 		while (j<max){
// 			bufferEstadistica[j]='*';
// 			j++;
// 		}
// 		bufferEstadistica[j] = '%';
		
// 		index = concatenar_array(bufferMsg, bufferEstadistica, index);
		
// 	}
// 	bufferMsg[index] = '\n';
// 	bufferMsg[index+1] = '%';
// 	int ind = concatenar_array(buffer,bufferMsg,0);
// 	buffer[ind] = '%';
// }



void mostrar_volver_a_jugar(char buffer[]){
	char bufferMsg[300] = "****************************\nINTRODUCE EL COMANDO $NEW! PARA VOLVER A JUGAR\n****************************\n\n%";
	int ind = concatenar_array(buffer,bufferMsg,0);
	buffer[ind] = '%';

	
}

//muestra el mensaje de movimiento cancelado por un jugador
void mostrar_movimiento_cancelado(){
	uint8_t bufferMsg[22] = "MOVIMIENTO CANCELADO\n%";
	linea_serie_drv_enviar_array(bufferMsg);
}



void mostrar_error_juego(){
	char bufferMsg[300] = "****************************\nINTRODUCE EL COMANDO $NEW! PARA VOLVER A JUGAR\n****************************\n\n%";
	linea_serie_drv_enviar_array(bufferMsg);
}

// funcion que muestra la pantalla final en la cual se encuentra la causa, tiempo total de uso del procesador, tiempo
// total y medio en computo de conecta_k_hay_linea, tiempo total y media de tiempo que al humano le cuesta pensar la jugada 
// y el total de eventos encolados en la cola de eventos.
void mostrar_pantalla_final_juego(){ //0 -> victoria, 1 -> cancel, 2 -> end
	char bufferMsgFinal[1000];
	char bufferMsgAux[300];
	int index = 0;

	mostrar_titulo_final_juego(bufferMsgAux);
	index = concatenar_array(bufferMsgFinal,bufferMsgAux,index);
	mostrar_causa(bufferMsgAux);
	index = concatenar_array(bufferMsgFinal,bufferMsgAux,index);
	// mostrar_tiempo_procesador(bufferMsgAux);
	// index = concatenar_array(bufferMsgFinal,bufferMsgAux,index);
	// mostrar_tiempo_hay_linea(bufferMsgAux);
	// index = concatenar_array(bufferMsgFinal,bufferMsgAux,index);
	// mostrar_tiempo_humano(bufferMsgAux);
	// index = concatenar_array(bufferMsgFinal,bufferMsgAux,index);
	// mostrar_estadisticas(bufferMsgAux);
	// index = concatenar_array(bufferMsgFinal,bufferMsgAux,index);
	// mostrar_volver_a_jugar(bufferMsgAux);
	// index = concatenar_array(bufferMsgFinal,bufferMsgAux,index);
	bufferMsgFinal[index] = '%';
	linea_serie_drv_enviar_array(bufferMsgFinal);

}




//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES PUBLICAS------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//comienzo del juego, inicializa el tablero y muestra mensaje inicial por pantalla
void juego_inicializar(void (*callback_fifo_encolar_param)(), void (*callback_gpio_hal_sentido_param)(), void (*callback_gpio_hal_escribir_param)(), int GPIO_JUGAR_ERROR_PARAM, int GPIO_JUGAR_ERROR_BITS_PARAM, int GPIO_HAL_PIN_DIR_OUTPUT_PARAM){
	cuenta = 0;
	intervalo = 0;
	callback_fifo_encolar = callback_fifo_encolar_param;
	GPIO_JUGAR_ERROR = GPIO_JUGAR_ERROR_PARAM;
	GPIO_JUGAR_ERROR_BITS = GPIO_JUGAR_ERROR_BITS_PARAM;
	callback_gpio_hal_sentido_param(GPIO_JUGAR_ERROR_PARAM, GPIO_JUGAR_ERROR_BITS_PARAM, GPIO_HAL_PIN_DIR_OUTPUT);
	tablero_inicializar(&cuadricula);
	conecta_K_test_cargar_tablero(&cuadricula); // igual habra que comentarlo, auqne en el enunciado habla algo sobre ello 
	char bufferMsgIni[] = "****************************\n\tCONECTA K\nPULSE UN BOTON PARA INICIAR\nO ESCRIBA EL COMANDO $NEW!\nPARA REALIZAR UNA JUGADA\nDEBE INTRODUCIR EL COMANDO ($#-#!)\nPARA FINALIZAR LA PARTIDA\nPULSE EL BOTON 2 O\nINTRODUZCA EL COMANDO $END!\n****************************\n\n%";
	state = ESCRITURA_PAG_PRINCIPAL;
	linea_serie_drv_enviar_array(bufferMsgIni);

}





//maquina de estados de nuestro juego
void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData){
	if (state == ESCRITURA_PAG_PRINCIPAL){
		if(ID_evento == ev_TX_SERIE){
			state = WAIT_INICIO_PARTIDA;
		}
		
	}else if (state == WAIT_INICIO_PARTIDA ){
		if (ID_evento == ev_RX_SERIE){ 
			uint8_t bufferTratarEvento[5];
			bufferTratarEvento[0] = (auxData >> 16 ) & 0xFF;
			bufferTratarEvento[1] = (auxData >> 8) & 0xFF;
			bufferTratarEvento[2] = (auxData) & 0xFF;
			bufferTratarEvento[3] = '\n';
			bufferTratarEvento[4] = '%';
			if((bufferTratarEvento[0] == 'N' && bufferTratarEvento[1] == 'E'&& bufferTratarEvento[2] == 'W')){
				turno = turnoEmpieza;
				if(turnoEmpieza == 1){
					turnoEmpieza = 2;
				}else{
					turnoEmpieza = 1;
				}
				t1Procesador = clock_get_us();
				conecta_K_visualizar_tablero_juego();
				state = ESCRITURA_MOSTRAR_TABLERO;
			}
		}else if(ID_evento == BOTON){
			if (primeraVez == 0){
				primeraVez = 1;
			}else{
				uint64_t tiempo_actual;
				tiempo_actual = temporizador_drv_leer();
				intervalo = tiempo_actual - ultimaPulsacion;
				ultimaPulsacion = tiempo_actual;
			}
			conecta_K_visualizar_tablero_juego();
			state = ESCRITURA_MOSTRAR_TABLERO;
		}
	}else if (state == ESCRITURA_MOSTRAR_TABLERO){
		if(ID_evento == ev_TX_SERIE){
			state = WAIT_COMANDO;
			t1Humano = clock_get_us();
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
				//callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0);
				reason = 2;
				t2Procesador = clock_get_us();
				mostrar_pantalla_final_juego();
				state = ESCRITURA_MOSTRAR_FIN;

			}else if((bufferTratarEvento[0] >= '1' && bufferTratarEvento[0] <= '7' && bufferTratarEvento[1] == '-' && bufferTratarEvento[2] >= '1' && bufferTratarEvento[2] <= '7')){
				fila = bufferTratarEvento[0] - '0';
				columna = bufferTratarEvento[2] - '0';
				if (celda_vacia(tablero_leer_celda(&cuadricula, fila -1, columna -1))){
					//callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0);
					conecta_K_visualizar_movimiento_juego();
					state = ESCRITURA_COMANDO_CORRECTO;
					alarma_activar(ev_JUEGO, 3000, 0);
				}else{
					//visualizar error
					callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 1);
					mostrar_error_juego();
					state = ESCRITURA_MOSTRAR_ERROR;
				}
			}else{
				callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 1);
				mostrar_error_juego();
				state = ESCRITURA_MOSTRAR_ERROR;
			}
		}else if(ID_evento == BOTON && auxData == 2){
			callback_gpio_hal_escribir(GPIO_JUGAR_ERROR, GPIO_JUGAR_ERROR_BITS, 0);
			uint64_t tiempo_actual;
			tiempo_actual = temporizador_drv_leer();
			intervalo = tiempo_actual - ultimaPulsacion;
			ultimaPulsacion = tiempo_actual;
			reason = 1;
			t2Procesador = clock_get_us();
			mostrar_pantalla_final_juego();
			state = ESCRITURA_MOSTRAR_FIN;

		}
	}else if (state == ESCRITURA_COMANDO_CORRECTO){
		if(ID_evento == ev_TX_SERIE){
			state = WAIT_CANCELAR; // MIRAR SI ESTA BIEN
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
	}else if (state == ESCRITURA_TABLERO_FIN){
		if(ID_evento == ev_TX_SERIE){
			mostrar_pantalla_final_juego();
			state = ESCRITURA_MOSTRAR_FIN;
		}
	}else if ( state == ESCRITURA_MOSTRAR_FIN){
		if(ID_evento == ev_TX_SERIE){
			state = WAIT_INICIO_PARTIDA;
		}
	}

	
}