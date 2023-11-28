#ifndef EVENTOS_H
#define EVENTOS_H


#include <inttypes.h> 

#define NUMEVENTOS 18



enum ID_EVENTO
{
    VOID = 0,
    TIMER = 1,
    GPIO = 2,
    HELLOWORLD = 3,
    ALARMA_OVERFLOW = 4,
    BOTON = 5,
    BOTON_EINT1_ALARM = 6,
    BOTON_EINT2_ALARM = 7,
	DEEP_SLEEP = 8,
    ev_VISUALIZAR_CUENTA = 9,
    ev_LATIDO = 10,
    ev_VISUALIZAR_HELLO = 11,
    ev_TEST = 12,
    TEST_OK = 13,
    UART0_CARACTER = 14,
    ev_RX_SERIE = 15,
    ev_TX_SERIE = 16,
    CONTINUAR_ENVIO = 17,
};

typedef uint8_t EVENTO_T;

typedef struct EVENTOS{
    EVENTO_T id;
    uint32_t auxData;
} EVENTOS;


#endif
