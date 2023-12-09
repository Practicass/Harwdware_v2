
#include "alarmas.h"

#define ALARMAS_MAX 4                                   //Numero maximo de alarmas

//Estructura de datos para almacenar las alarmas
struct alarm{
    int periodica;                                      //booleano 1 = es peridodica 0 = no perisodica
    int contador;                                       // numero de interrupciones desde la programacion de la alarma 
    EVENTO_T evento;                                    //evento asociado a la alarma                                   
    uint32_t auxdata;                                   //dato auxiliar asociado a la alarma       
    int periodo;                                        // numero de interrupciones hasta
    int ocupado;                                        // ocupado = 0 NO hay una alarma activa, ocupado = 1 existe una alarma activa
};


static struct alarm alarmas[ALARMAS_MAX];               //vector de alarmas

static int indexAlarms[NUMEVENTOS];                     //vector que guarda el indice del vector de alarmas de la alarma de cada evento, si no existe alarma para ese evento guardara un -1

static void (*callback_fifo_encolar)();                 //puntero a funcion que encola un evento en la fifo de eventos



//---------------------------------------------------------------------------------------------------------------------
//---------------------------------FUNCIONES --------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//Función que inicializa las estructuras de datos necesarias para el funcionamiento de las alarmas 
//y activa un temporizador periodico cada 1ms
void alarma_inicializar(void (*callback_fifo_encolar_param)()){

    int i = 0;
    while (i< ALARMAS_MAX)
    {   
        //inicializar los registros
        alarmas[i].contador = 0;
        alarmas[i].periodica = 0;
        alarmas[i].ocupado = 0;
        alarmas[i].evento = 0;
        alarmas[i].auxdata = 0;
				i++;
    }
    i = 0;
    while (i <NUMEVENTOS)
    {
        indexAlarms[i] = -1; //-1 indica que no existe alamra de ese evento
				i++;
    }
        
		temporizador_drv_reloj(1, callback_fifo_encolar_param, TIMER);
        callback_fifo_encolar = callback_fifo_encolar_param;
    
    
}

//Función que activa una alarma para un evento con un retardo determinado. Si el retardo es 0 se desactiva la alarma.
//Si la alarma ya estaba activa se resetea.
void alarma_activar(EVENTO_T ID_evento, uint32_t retardo, uint32_t auxData){
    int i = 0;
    int periodico;                                //booleano 1 = es peridodica 0 = no perisodica
    int retardoAux;                               //retardo sin el bit de mas peso 
    int id;                                       //indice de la alarma en el vector de alarmas

    //comprobar si retardo = 0 y por tanto se debe desactivar la alarma
    if(retardo == 0){ 
        
        //se quita la alarma correspondiente de ese evento del vector de alarmas
        id = indexAlarms[ID_evento];
        indexAlarms[ID_evento] = -1;

        //se reseta la alarma
        alarmas[id].contador = 0;
        alarmas[id].periodica = 0;
        alarmas[id].ocupado = 0;
        alarmas[id].evento = 0;
        alarmas[id].auxdata = 0;

    }else{
        //comprobar si la alarma es periodica, mirando el bit de mas peso en retardo
        periodico = retardo & 0x80000000;
        retardoAux = retardo & 0x7fffffff;
        //comprobacion de la existencia de una alarma para ID_evento
        if(indexAlarms[ID_evento] == -1){                   //no existe alarma para ese evento
        //buscar una alarma disponible en el vector de alarmas
            while(i<ALARMAS_MAX){
                if(alarmas[i].ocupado == 0){
                    alarmas[i].evento = ID_evento;
                    indexAlarms[ID_evento] = i;
                    alarmas[i].auxdata = auxData;
                    alarmas[i].ocupado = 1;
                    alarmas[i].periodica = (periodico != 0);
                    alarmas[i].periodo = retardoAux;
									
					i = ALARMAS_MAX +1; //para salir del bucle
                }

                i++;
            }
            if (i == ALARMAS_MAX){                           //no hay alarmas disponibles
                
                callback_fifo_encolar(ALARMA_OVERFLOW, 0); //generar evento ALARMA_OVERFLOW
            }
            
        }else{ //existe alarma para ese evento y por tanto se resetea
            id = indexAlarms[ID_evento];
            alarmas[id].contador = 0;
            alarmas[id].auxdata = auxData;
            alarmas[id].periodica = (periodico != 0);
            alarmas[id].periodo = retardoAux;
        }
    }


}

//Función que se llama cada vez que se produce una interrupción del temporizador. Se encarga de comprobar si alguna 
//alarma ha expirado y en ese caso encola el evento correspondiente en la fifo de eventos.
void alarma_tratar_evento(void){
    int i = 0;
	int id;
    while(i<ALARMAS_MAX){
        //comprobar si existe alarma
        if(alarmas[i].ocupado == 1){                        //existe alarma
            alarmas[i].contador++ ;                         //incrementar contador
            if(alarmas[i].contador >= alarmas[i].periodo){  //comprobar si ha expirado la alarma
                //se encola el evento correspondiente en la fifo de eventos
                callback_fifo_encolar(alarmas[i].evento, alarmas[i].auxdata);
                //se comprueba si la alarma es periodica
                if(alarmas[i].periodica == 0){              //no es periodica
                    //se quita la alarma correspondiente de ese evento del vector de alarmas
                    id = alarmas[i].evento;
                    indexAlarms[id] = -1;

                    //se reseta la alarma
                    alarmas[i].contador = 0;
                    alarmas[i].periodica = 0;
                    alarmas[i].ocupado = 0;
                    alarmas[i].auxdata = 0;
                    alarmas[i].evento = 0;
                }else{
                    alarmas[i].contador = 0;                //se resetea el contador
                }
            }
        }
				i++;                                        //incrementar i para pasar comprobar la siguiente alarma
    }
}






