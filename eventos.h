#ifndef EVENTOS_H
#define EVENTOS_H


#include <inttypes.h> 

#define NUMEVENTOS 13



enum ID_EVENTO
{
    VOID = 0,
    TIMER = 1,
    ALARMA_OVERFLOW = 2,
    BOTON = 3,
    BOTON_EINT1_ALARM = 4,
    BOTON_EINT2_ALARM = 5,
	DEEP_SLEEP = 6,
    ev_VISUALIZAR_CUENTA = 7,
    ev_LATIDO = 8,
    ev_VISUALIZAR_HELLO = 9,
    ev_RX_SERIE = 10,
    ev_TX_SERIE = 11,
    ev_JUEGO = 12,
};

typedef uint8_t EVENTO_T;

typedef struct EVENTOS{
    EVENTO_T id;
    uint32_t auxData;
} EVENTOS;


#endif
