
#include "juego.h"

static volatile int cuenta;
static volatile int intervalo;
static volatile int ultimaPulsacion = 0;
static volatile int primeraVez = 0;
static void (*callback_fifo_encolar)();
static uint8_t salida[8][8];
static TABLERO cuadricula;
static uint32_t t1;
int last_command_tab = 0; // boolean

void juego_inicializar(void (*callback_fifo_encolar_param)()){
	cuenta = 0;
	intervalo = 0;
	callback_fifo_encolar = callback_fifo_encolar_param;



	tablero_inicializar(&cuadricula);
	conecta_K_test_cargar_tablero(&cuadricula);



}

void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData){
	if (ID_evento == ev_RX_SERIE){
		uint8_t bufferTratarEvento[5];
		bufferTratarEvento[0] = (auxData >> 16 ) & 0xFF;
		bufferTratarEvento[1] = (auxData >> 8) & 0xFF;
		bufferTratarEvento[2] = (auxData) & 0xFF;
		bufferTratarEvento[3] = '\n';
		bufferTratarEvento[4] = '%';
		if((bufferTratarEvento[0] == 'E' && bufferTratarEvento[1] == 'N'&& bufferTratarEvento[2] == 'D')||(bufferTratarEvento[0] == 'N' && bufferTratarEvento[1] == 'E'&& bufferTratarEvento[2] == 'W')||
			(bufferTratarEvento[0] >= '1' && bufferTratarEvento[0] <= '7' && bufferTratarEvento[1] == '-' && bufferTratarEvento[2] >= '1' && bufferTratarEvento[2] <= '7')){
				linea_serie_drv_enviar_array(bufferTratarEvento);

		}else if((bufferTratarEvento[0] == 'T' && bufferTratarEvento[1] == 'A'&& bufferTratarEvento[2] == 'B')){
			last_command_tab = 1;
			conecta_K_visualizar_tablero_juego();
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


		if(auxData == 1){
			cuenta++;
		}else{
			cuenta--;
		}
		callback_fifo_encolar(ev_VISUALIZAR_CUENTA, cuenta);
	}else if(ID_evento == ev_TX_SERIE){
		if(last_command_tab == 1){
			last_command_tab = 0;
			conecta_K_visualizar_tiempo(auxData-t1);
		}
	}


}

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

void tiempo_visualizar_tablero(uint32_t t2){

	uint32_t t3;
	t3 = t2-t1;
	conecta_K_visualizar_tiempo(t3);
}

void conecta_K_visualizar_tiempo(uint32_t num){
	int numAux = num;
	unsigned int longitud = 0;
	 uint8_t array_digitos[300];
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
	linea_serie_drv_enviar_array(array_digitos);
}




