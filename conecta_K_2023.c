#include "conecta_K_2023.h"
#include "entrada.h"


extern uint8_t conecta_K_buscar_alineamiento_arm(TABLERO *t, uint8_t fila,
	uint8_t columna, uint8_t color, int8_t delta_fila, int8_t
	delta_columna);

extern uint8_t conecta_K_hay_linea_arm_c(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);

extern uint8_t conecta_K_hay_linea_arm_arm(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color);

// devuelve la longitud de la línea más larga en un determinado sentido
uint8_t conecta_K_buscar_alineamiento_c(TABLERO *t, uint8_t fila,
	uint8_t columna, uint8_t color, int8_t delta_fila, int8_t
	delta_columna)
{
uint8_t longitud = 0;
    
    while (tablero_buscar_color(t, fila, columna, color) == EXITO) {
        
        // Incrementar la longitud y avanzar a la celda vecina
        longitud++;
        fila += delta_fila;
        columna += delta_columna;
    }

    return longitud;
}

// conecta_K_hay_linea_c_c sin recursividad 
uint8_t
conecta_K_hay_linea_c_c2(TABLERO *t, uint8_t fila, uint8_t columna, uint8_t color)
{
	 enum { N_DELTAS = 4};
   int8_t deltas_fila[N_DELTAS] = {0, -1, -1, 1};
   int8_t deltas_columna[N_DELTAS] = {-1, 0, -1, -1};
   unsigned int i = 0;
   uint8_t linea = FALSE;
   uint8_t long_linea = 0;
   unsigned int aux = 0;
   uint8_t fila2=0;
   uint8_t columna2=0;
   uint8_t delta_columna2=0;
   uint8_t delta_fila2=0;
   int8_t res=0;
   

   // buscar linea en fila, columna y 2 diagonales
   while((i < N_DELTAS) && (linea == FALSE)){
        if(aux % 2){
            fila2 = fila-deltas_fila[i];
            columna2 = columna-deltas_columna[i];
            delta_fila2 = -deltas_fila[i];
            delta_columna2 = -deltas_columna[i];
            i++;
        }else{
            fila2 = fila;
            columna2 = columna;
            delta_fila2 = deltas_fila[i];
            delta_columna2 = deltas_columna[i];
						long_linea = 0;
            
        }
        aux ++;


        uint8_t longitud = 0;

        do{
            // comprobar validez posición
            if( fila2 >= NUM_FILAS || columna2 >= NUM_COLUMNAS) {
                res = -1;
            }else{

                // buscar columna en estructura dispersa
                size_t col = 0;
                for(; (col < MAX_NO_CERO) && (t->columnas[fila2][col] != columna2); ++col);

                if(col == MAX_NO_CERO) {
                    res =  -1;
                }else{

                    //si mismo color al que busco devolver encontrada
                    if((t->no_ceros[fila2][col] & 0x03) == color) {
                        res =  0;
											  longitud++;
												fila2 += delta_fila2;
												columna2 += delta_columna2;
                    }else{
                                            
                        // devolver error si posicion invalida y distinto color
                        res= -1;
                    }

                }
            }
        }while ( res == EXITO);

        long_linea += longitud;
       linea = long_linea >= K_SIZE;

   }
   return linea;

}

// devuelve true si encuentra una línea de longitud mayor o igual a 4
uint8_t conecta_K_hay_linea_c_c(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color){
		 enum { N_DELTAS = 4};
   int8_t deltas_fila[N_DELTAS] = {0, -1, -1, 1};
   int8_t deltas_columna[N_DELTAS] = {-1, 0, -1, -1};
   unsigned int i = 0;
   uint8_t linea = FALSE;
   uint8_t long_linea = 0;

   // buscar linea en fila, columna y 2 diagonales
   for(i=0; (i < N_DELTAS) && (linea == FALSE); ++i) {
       // buscar sentido
       long_linea = conecta_K_buscar_alineamiento_c(t, fila, columna, color, deltas_fila[i], deltas_columna[i]);
       linea = long_linea >= K_SIZE;
       if (linea) {
         continue;
       }
       // buscar sentido inverso
       long_linea += conecta_K_buscar_alineamiento_c(t, fila-deltas_fila[i],
	       columna-deltas_columna[i], color, -deltas_fila[i], -deltas_columna[i]);
       linea = long_linea >= K_SIZE;
   }
   return linea;
		
	}



// devuelve true si encuentra una línea de longitud mayor o igual a 4
uint8_t conecta_K_hay_linea_c_arm(TABLERO *t, uint8_t fila, uint8_t
	columna, uint8_t color){
		 enum { N_DELTAS = 4};
   int8_t deltas_fila[N_DELTAS] = {0, -1, -1, 1};
   int8_t deltas_columna[N_DELTAS] = {-1, 0, -1, -1};
   unsigned int i = 0;
   uint8_t linea = FALSE;
   uint8_t long_linea = 0;

   // buscar linea en fila, columna y 2 diagonales
   for(i=0; (i < N_DELTAS) && (linea == FALSE); ++i) {
       // buscar sentido
       long_linea = conecta_K_buscar_alineamiento_arm(t, fila, columna, color, deltas_fila[i], deltas_columna[i]);
       linea = long_linea >= K_SIZE;
       if (linea) {
         continue;
       }
       // buscar sentido inverso
       long_linea += conecta_K_buscar_alineamiento_arm(t, fila-deltas_fila[i],
	       columna-deltas_columna[i], color, -deltas_fila[i], -deltas_columna[i]);
       linea = long_linea >= K_SIZE;
   }
   return linea;
		
	}


// carga el estado a mitad de partida de las primeras 7 filas y columnas 
// a la estructura de datos tablero para facilitar el test de posibles jugadas
//
// 0: casilla vacia, 1:ficha jugador uno, 2: ficha jugador dos
void conecta_K_test_cargar_tablero(TABLERO *t)
{
	#include "tablero_test.h"	

	//for... for... tablero_insertar_color...	
	for(int i=0; i<7; i++){
		int n=0;
		for(int j=0; j<7; j++){
			int color = tablero_test[i][j];
			if( color != 0){
				tablero_insertar_color(t,i,j,color);
				n++;
			}
		}
	}

}

// funcion que visualiza en "pantalla" el contenido de las 7 primeras filas y columnas 
// de las m*n del tablero en juego (suponemos que m y n son >= 7 siempre)
// en memoria se deberia ver algo tal que asi:
// 00 C1 C2 C3 C4 C5 C6 C7
// F1 00 00 00 00 00 00 00
// F2 00 00 11 00 00 00 00
// F3 00 11 22 22 00 00 00
// F4 00 00 11 00 00 00 00
// F5 00 00 00 00 00 00 00
// F6 00 00 00 00 00 00 00
// F7 00 00 00 00 00 00 00 
void conecta_K_visualizar_tablero(TABLERO *t, uint8_t pantalla[8][8])
{
//.... tablero_leer_celda...
//... celda_color ....
	for(int x=1; x<8; x++){
		pantalla[x][0] = 240 + x;
	}
	for(int y=1; y<8; y++){
		pantalla[0][y] = 192 + y;
	}
	for(int i=0; i<7; i++){
		for(int j=0; j<7; j++){
			int color=celda_color(tablero_leer_celda(t,i,t->columnas[i][j]));
			pantalla[i+1][t->columnas[i][j]+1] = color*16+color;
		}
	}
}  

//
int conecta_K_verificar_K_en_linea(TABLERO *t, uint8_t fila, uint8_t columna, uint8_t color){
	// en esta funcion es donde se debe verificar que todas las optimizaciones dan el mismo resultado
	uint8_t resultado_c_c = conecta_K_hay_linea_c_arm(t, fila, columna, color);
	uint8_t resultado_arm_arm = conecta_K_hay_linea_c_c(t, fila, columna, color);

	if(resultado_arm_arm != resultado_c_c) while(1);

	return resultado_arm_arm;
}

void conecta_K_jugar(void){
	// new, row, column, colour, padding to prevent desalinating to 8 bytes
	static volatile uint8_t entrada[8] = {0, 0, 0, 0, 0, 0, 0, 0 }; //jugada, fila, columna, color, ...
	// 8x8 intentando que este alineada para que se vea bien en memoria
	static uint8_t salida[8][8];
	
	TABLERO cuadricula;

	uint8_t row, column, colour;

	tablero_inicializar(&cuadricula);

	conecta_K_test_cargar_tablero(&cuadricula);
	conecta_K_visualizar_tablero(&cuadricula, salida);


	entrada_inicializar(entrada);
	
	while (1){
		while (entrada_nueva(entrada) == 0){};
		entrada_leer(entrada, &row, &column, &colour);
		//validada la entrada en rango, mirar color valido?
		if(tablero_fila_valida(row) && tablero_columna_valida(column) && tablero_color_valido(colour)){	
			//podriamos no validarla ya que tablero_insertar_valor vuelve a validar
			if (celda_vacia(tablero_leer_celda(&cuadricula, row, column))){
				//tablero_insertar tambien chequea si esta libre esa celda o no...
				if(tablero_insertar_color(&cuadricula, row, column, colour) == EXITO) {
					conecta_K_visualizar_tablero(&cuadricula, salida);
					if(conecta_K_verificar_K_en_linea(&cuadricula, row, column, colour)) {
						while(1); // equivaldria a K_linea encontrada, fin de partida... 
					}
				}
				else {
					while(1); //no cabe en la matriz dispersa, hemos dimensionado mal, error de diseño
				}
			}
			//else: celda no vacia
		}
		//else: fuera de rango fila, columna o color
		entrada_inicializar (entrada);
	}
}






//-----------------------------------------------------------------------------------------
//-----------------FUNCIONES PARA REALIZAR EL TESTING -------------------------------------
//-----------------------------------------------------------------------------------------


// Pone todas las casillas del tablero t a 0, es decir quita todas las fichas del tablero
void limpiar_tablero(TABLERO *t){
		for(int i=0; i<7; i++){
		for(int j=0; j<7; j++){
				tablero_borrar_celda(t,i,j);
			}
		}
	
}


// Escribe en memoria el nuevo tablero vacio
void visualizar_nuevo_tablero( uint8_t pantalla[8][8]){
			for(int i=0; i<7; i++){
			for(int j=0; j<7; j++){
				pantalla[i+1][j+1] = 0;
			}
		}
}

// Carga diferentes combinaciones de jugadas para comprobar el correcto funcionamiento de nuestra función
void cargar_nueva_entrada( volatile uint8_t *entrada, int i){
	static volatile uint8_t matriz_entradas[60][4] = {
		1,1,1,1,
		1,3,2,1,
		1,4,4,1,
		
		1,6,6,2,
		1,5,6,2,
		1,4,6,2,
		1,3,6,2,
		
		1,2,2,1,
		1,2,3,1,
		1,2,4,1,
		1,2,5,1,
		
		1,2,2,1,
		1,3,3,2,
		1,2,3,1,
		1,2,4,2,
		1,3,2,1,
		1,4,2,2,
		1,5,1,1,
		1,1,5,2,
		
		1,4,4,1,
		1,3,4,2,
		1,4,3,1,
		1,4,2,2,
		1,5,3,1,
		1,3,3,2,
		1,6,3,1,
		1,7,3,2,
		1,4,5,1,
		1,4,6,2,
		1,5,5,1,
		1,3,5,2,
		1,6,5,1,
		1,3,6,2,


		1,7,1,1,
		1,7,2,1,
		1,7,3,1,
		1,7,4,1,


		1,7,1,1,
		1,6,1,1,
		1,5,1,1,
		1,4,1,1,


		1,7,7,2,
		1,6,6,2,
		1,5,5,2,
		1,4,4,2,



		
		1,1,1,1,
		1,1,2,1,
		1,1,3,1,
		1,1,4,1,


		1,1,1,1,
		1,2,1,1,
		1,3,1,1,
		1,4,1,1,


		1,1,1,2,
		1,2,2,2,
		1,3,3,2,
		1,4,4,2,
	};
	

	for(int j=0; j<4; j++){
		entrada[j] = matriz_entradas[i][j];
	}
	
}



void conecta_K_test(void){
	// new, row, column, colour, padding to prevent desalinating to 8 bytes
	static volatile uint8_t entrada[8] = {0, 0, 0, 0, 0, 0, 0, 0 }; //jugada, fila, columna, color, ...
	// 8x8 intentando que este alineada para que se vea bien en memoria
	static uint8_t salida[8][8];

	int i = 0;
	
	TABLERO cuadricula;

	uint8_t row, column, colour;

	tablero_inicializar(&cuadricula);

	conecta_K_test_cargar_tablero(&cuadricula);
	conecta_K_visualizar_tablero(&cuadricula, salida);


	entrada_inicializar(entrada);
	
	while (1){
		cargar_nueva_entrada(entrada, i);
		i++;
		while (entrada_nueva(entrada) == 0){};
		entrada_leer(entrada, &row, &column, &colour);
		//validada la entrada en rango, mirar color valido?
		if(tablero_fila_valida(row) && tablero_columna_valida(column) && tablero_color_valido(colour)){	
			//podriamos no validarla ya que tablero_insertar_valor vuelve a validar
			if (celda_vacia(tablero_leer_celda(&cuadricula, row, column))){
				//tablero_insertar tambien chequea si esta libre esa celda o no...
				if(tablero_insertar_color(&cuadricula, row, column, colour) == EXITO) {
					conecta_K_visualizar_tablero(&cuadricula, salida);
					if(conecta_K_verificar_K_en_linea(&cuadricula, row, column, colour)) {
						//while(1); // equivaldria a K_linea encontrada, fin de partida... 
						//fin = 1;
						limpiar_tablero(&cuadricula);
						visualizar_nuevo_tablero(salida);
						
						//tablero_inicializar(&cuadricula);
						//conecta_K_test_cargar_tablero(&cuadricula);

						conecta_K_visualizar_tablero(&cuadricula, salida);
					}
				}
				else {
					while(1); //no cabe en la matriz dispersa, hemos dimensionado mal, error de diseño
				}
			}
			//else: celda no vacia
		}
		//else: fuera de rango fila, columna o color
		entrada_inicializar (entrada);
	}
}

