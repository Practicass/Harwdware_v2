
#include "juego.h"

static volatile int cuenta;
static volatile int intervalo;
static volatile int ultimaPulsacion = 0;
static volatile int primeraVez = 0;
static void (*callback_fifo_encolar)();
static uint8_t salida[8][8];
static TABLERO cuadricula;
static uint32_t t1;
static int last_command_tab = 0; // boolean
static uint32_t tiempo_conecta_k_hay_linea = 0;
static uint32_t tiempo_humano_piensa_jugada = 0;
static uint32_t contador_conecta_k_hay_linea = 0;
static uint32_t contador_humano_piensa_jugada = 0;
static uint8_t turno = 1;
static uint8_t fila = 0;
static uint8_t columna = 0;


	
enum ESTADOS{ 
	PAG_PRINCIPAL = 0,
    ESCRITURA_PAG_PRINCIPAL = 1,
    WAIT_INICIO_PARTIDA = 2,
    //MOSTRAR_TABLERO = 3,
    ESCRITURA_MOSTRAR_TABLERO = 4,
    WAIT_COMANDO = 5,
	//MOSTRAR_ERROR = 6,
	ESCRITURA_MOSTAR_ERROR = 7,
	//MOSTRAR_FIN = 8,
	ESCRITURA_MOSTRAR_FIN = 9,
	COMANDO_CORRECTO = 10,
	ESCRITURA_COMANDO_CORRECTO = 11,
	WAIT_CANCELAR = 12,
	CANCELADO = 13,
	REALIZAR_COMANDO = 14
	}

static  state = PAG_PRINCIPAL;
void juego_inicializar(void (*callback_fifo_encolar_param)()){
	cuenta = 0;
	intervalo = 0;
	callback_fifo_encolar = callback_fifo_encolar_param;



	tablero_inicializar(&cuadricula);
	conecta_K_test_cargar_tablero(&cuadricula); // igual habra que comentarlo, auqne en el enunciado habla algo sobre ello 


	// char bufferMsgIni[800] = {
	// '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','\n',
    // '\t', 'C', 'O', 'N', 'E', 'C', 'T', 'A', ' ', 'K', '\n',
    // 'P', 'U', 'L', 'S', 'E', ' ', 'U', 'N', ' ', 'B', 'O', 'T', 'O', 'N', ' ', 'P', 'A', 'R', 'A', ' ', 'I', 'N', 'I', 'C', 'I', 'A', 'R', '\n', 
    // 'O', ' ', 'E', 'S', 'C', 'R', 'I', 'B', 'A', ' ', 'E', 'L', ' ', 'C', 'O', 'M', 'A', 'N', 'D', 'O', ' ', '$','N', 'E', 'W','!', '\n',
    // 'P', 'A', 'R', 'A', ' ', 'R', 'E', 'A', 'L', 'I', 'Z', 'A', 'R', ' ',
    // 'U', 'N', 'A', ' ', 'J', 'U', 'G', 'A', 'D', 'A', ' \n', 'D', 'E', 'B', 'E', ' ',
    // 'I', 'N', 'T', 'R', 'O', 'D', 'U', 'C', 'I', 'R', ' ', 'E','L',' ','\n',
	// 'C', 'O', 'M', 'A', 'N', 'D', 'O', ' ', '(', '$', '#', '-', '#', '!', ')','\n',
	//  'P', 'A', 'R', 'A', ' ', 'F', 'I', 'N', 'A', 'L', 'I', 'Z', 'A', 'R', ' ',
    // 'L', 'A', ' ', 'P', 'A', 'R', 'T', 'I', 'D', 'A', '\n',
	// 'P', 'U', 'L', 'S', 'E', ' ','E', 'L', ' ', 'B', 'O', 'T', 'O', 'N', ' ', '2', ' ', 'O', '\n', 'I', 'N', 'T', 'R', 'O', 'D', 'U', 'Z', 'C', 'A', ' ', 'E', 'L', ' ',
	//  'C', 'O', 'M', 'A', 'N', 'D', 'O', ' ', '$', 'E', 'N', 'D', '!','\n',
	// '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','\n','\n','%'
	// };
	char bufferMsgIni[] = "****************************\n\tCONECTA K\nPULSE UN BOTON PARA INICIAR\nO ESCRIBA EL COMANDO $NEW!\nPARA REALIZAR UNA JUGADA\nDEBE INTRODUCIR EL COMANDO ($#-#!)\nPARA FINALIZAR LA PARTIDA\nPULSE EL BOTON 2 O\nINTRODUZCA EL COMANDO $END!\n****************************\n\n%";
	state = ESCRITURA_PAG_PRINCIPAL;
	linea_serie_drv_enviar_array(bufferMsgIni);



}


// funcion que muestra la pantalla final en la cual se encuentra la causa, tiempo total de uso del procesador, tiempo
// total y medio en computo de conecta_k_hay_linea, tiempo total y media de tiempo que al humano le cuesta pensar la jugada 
// y el total de eventos encolados en la cola de eventos.
void mostar_pantalla_final_juego(int reason){ // reason = 1 -> cancel, reason = 0 -> end
	
	if (reason == 1){
		char bufferMsgFin[73] = "\n\n\n\n****************************\n\tFIN DE LA PARTIDA\n\tPARTIDA CANCELADA\n%";
		linea_serie_drv_enviar_array(bufferMsgFin);
	}else{
		char bufferMsgFin[74] = "\n\n\n\n****************************\n\tFIN DE LA PARTIDA\n\tPARTIDA FINALIZADA\n%";
		linea_serie_drv_enviar_array(bufferMsgFin);
	}
	
	
	
}

void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData){
	if (state == ESCRITURA_PAG_PRINCIPAL)
	{
		state = WAIT_INICIO_PARTIDA;
	}else if (state == WAIT_INICIO_PARTIDA )
	{
		if (ID_evento == ev_RX_SERIE){ 
			uint8_t bufferTratarEvento[5];
			bufferTratarEvento[0] = (auxData >> 16 ) & 0xFF;
			bufferTratarEvento[1] = (auxData >> 8) & 0xFF;
			bufferTratarEvento[2] = (auxData) & 0xFF;
			bufferTratarEvento[3] = '\n';
			bufferTratarEvento[4] = '%';
			if((bufferTratarEvento[0] == 'N' && bufferTratarEvento[1] == 'E'&& bufferTratarEvento[2] == 'W')){
				//linea_serie_drv_enviar_array(bufferTratarEvento); //preguntar si hay que mostrar el comando
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
		state = WAIT_COMANDO;
	}else if (state == WAIT_COMANDO){
		if (ID_evento == ev_RX_SERIE){ 
			uint8_t bufferTratarEvento[5];
			bufferTratarEvento[0] = (auxData >> 16 ) & 0xFF;
			bufferTratarEvento[1] = (auxData >> 8) & 0xFF;
			bufferTratarEvento[2] = (auxData) & 0xFF;
			bufferTratarEvento[3] = '\n';
			bufferTratarEvento[4] = '%';
			if((bufferTratarEvento[0] == 'E' && bufferTratarEvento[1] == 'N'&& bufferTratarEvento[2] == 'D')){
				mostar_pantalla_final_juego(1);
				state = ESCRITURA_MOSTRAR_FIN;
			}else if((bufferTratarEvento[0] >= '1' && bufferTratarEvento[0] <= '7' && bufferTratarEvento[1] == '-' && bufferTratarEvento[2] >= '1' && bufferTratarEvento[2] <= '7')){
				fila = bufferTratarEvento[0] - '0';
				columna = bufferTratarEvento[2] - '0';
				conecta_K_visualizar_movimiento_juego();
				state = ESCRITURA_COMANDO_CORRECTO;
				alarma_activar(ev_JUEGO, 3000, 0);
			}
		}else if(ID_evento == BOTON && auxData == 2){
			
			uint64_t tiempo_actual;
			tiempo_actual = temporizador_drv_leer();
			intervalo = tiempo_actual - ultimaPulsacion;
			ultimaPulsacion = tiempo_actual;
			
			mostar_pantalla_final_juego(1);
			state = ESCRITURA_MOSTRAR_FIN;
		}
	}else if (state == ESCRITURA_COMANDO_CORRECTO){
		state = WAIT_CANCELAR;
	}else if (state == WAIT_CANCELAR){
		if(ID_evento == ev_JUEGO){
			state = WAIT_COMANDO;
			//if (celda_vacia(tablero_leer_celda(&cuadricula, fila, columna))){
			//	//tablero_insertar tambien chequea si esta libre esa celda o no...
			//	if(tablero_insertar_color(&cuadricula, fila, columna, color) == EXITO) {
			//		conecta_K_visualizar_tablero(&cuadricula, salida);
			//	}
			//}
		}else if(ID_evento == BOTON){
			mostrar_movimiento_cancelado();
			state = ESCRITURA_COMANDO_CORRECTO;
		}
	}
	
	
	
	
	

}


void mostrar_movimiento_cancelado(){
	uint8_t bufferMsg[22] = "MOVIMIENTO CANCELADO\n%";
	linea_serie_drv_enviar_array(bufferMsg);
}
// if (ID_evento == ev_RX_SERIE){
	// 	uint8_t bufferTratarEvento[5];
	// 	bufferTratarEvento[0] = (auxData >> 16 ) & 0xFF;
	// 	bufferTratarEvento[1] = (auxData >> 8) & 0xFF;
	// 	bufferTratarEvento[2] = (auxData) & 0xFF;
	// 	bufferTratarEvento[3] = '\n';
	// 	bufferTratarEvento[4] = '%';
	// 	if((bufferTratarEvento[0] == 'E' && bufferTratarEvento[1] == 'N'&& bufferTratarEvento[2] == 'D')||(bufferTratarEvento[0] == 'N' && bufferTratarEvento[1] == 'E'&& bufferTratarEvento[2] == 'W')||
	// 		(bufferTratarEvento[0] >= '1' && bufferTratarEvento[0] <= '7' && bufferTratarEvento[1] == '-' && bufferTratarEvento[2] >= '1' && bufferTratarEvento[2] <= '7')){
	// 			linea_serie_drv_enviar_array(bufferTratarEvento);

	// 	}else if((bufferTratarEvento[0] == 'T' && bufferTratarEvento[1] == 'A'&& bufferTratarEvento[2] == 'B')){
	// 		last_command_tab = 1;
	// 		conecta_K_visualizar_tablero_juego();
	// 	}
	// }else if(ID_evento == BOTON){
	// 	if (primeraVez == 0){
	// 		primeraVez = 1;
	// 	}else{
	// 		uint64_t tiempo_actual;
	// 		tiempo_actual = temporizador_drv_leer();
	// 		intervalo = tiempo_actual - ultimaPulsacion;
	// 		ultimaPulsacion = tiempo_actual;
	// 	}


	// 	if(auxData == 1){
	// 		cuenta++;
	// 	}else{
	// 		cuenta--;
	// 	}
	// 	callback_fifo_encolar(ev_VISUALIZAR_CUENTA, cuenta);
	// }else if(ID_evento == ev_TX_SERIE){
	// 	if(last_command_tab == 1){
	// 		last_command_tab = 0;
	// 		conecta_K_visualizar_tiempo(auxData-t1);
	// 	}else if(state = 1){ // se ha escrito el mensaje inicial completamente
	// 		state ++;
	// 	}
	// }


void conecta_K_visualizar_tablero_juego()
{
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
	bufferTablero[136] = '%';
	linea_serie_drv_enviar_array(bufferTablero);

	
}

void conecta_K_visualizar_movimiento_juego() // se puede llamar a una funcion nueva que sea parecida a conecta k visualizar tablero juego
{
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
	bufferTablero[136] = '%';
	bufferTablero[fila*8*2+columna*2+fila] = 'x';
	linea_serie_drv_enviar_array(bufferTablero);

	
}

void tiempo_visualizar_tablero(uint32_t t2){

	uint32_t t3;
	t3 = t2-t1;
	conecta_K_visualizar_tiempo(t3);
}



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

void conecta_K_visualizar_tiempo(uint32_t num){
	char array_digitos[300];
	convesor_entero_char(num,array_digitos);
	linea_serie_drv_enviar_array(array_digitos);
}






