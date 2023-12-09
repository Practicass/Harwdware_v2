
#ifndef WD_HAL_H
#define WD_HAL_H

#include <LPC210X.H>  
#include "swi.h"

//inicializar el watchdog timer para que resetee el procesador dentro de sec segundos si no se "alimenta"
void WD_hal_inicializar(int sec);
//alimentar el watchdog
void WD_hal_feed(void);
//test con un bucle infinito y compurbear que se resetea y por tanto que el watchdog funciona
void WD_hal_test(void);

#endif

