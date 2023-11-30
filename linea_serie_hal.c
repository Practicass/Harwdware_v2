
#include "linea_serie_hal.h"

static void (*callback_linea_serie_drv_leer)();
static void (*callback_linea_serie_drv_escribir)();


void linea_serie_ISR(void) __irq{
    uint32_t iterrupcion = U0IIR;
    if((iterrupcion & 0x0e) == 4){
        callback_linea_serie_drv_leer(U0RBR);
    }else if((iterrupcion & 0x0e)==2){
        U0IER = 0x1;
        callback_linea_serie_drv_escribir();
    }
    VICVectAddr = 0;
}

void linea_serie_hal_inicializar(void (*callback_linea_serie_drv_leer_param)(), void (*callback_linea_serie_drv_escribir_param)()){
    callback_linea_serie_drv_leer = callback_linea_serie_drv_leer_param;
    callback_linea_serie_drv_escribir = callback_linea_serie_drv_escribir_param;
    PINSEL0 |= 0x5;
    U0LCR = 0x80;
    U0DLL = 91;
    U0LCR = 3;
    U0IER = 0x3;
    VICVectCntl4 = 0x20 | 6;
    VICVectAddr4 = (unsigned long)linea_serie_ISR;
    VICIntEnable |= 0x00000040;

}

void linea_serie_hal_escribir(uint8_t caracter){
    U0IER |= 0x2;
    U0THR = caracter;

}

