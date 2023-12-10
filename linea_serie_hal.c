
#include "linea_serie_hal.h"

static void (*callback_linea_serie_drv_leer)();                 //puntero a la funcion que lee el caracter introducido por el usuario y realiza su tratamiento
static void (*callback_linea_serie_drv_continuar_envio)();             //puntero a la funcion que lee el caracter introducido por el usuario y realiza su tratamiento

//rutina de tratamiento de la interrupción UART0
void linea_serie_ISR(void) __irq{
    uint32_t iterrupcion = U0IIR;                   //Se guarda el registro de identifiación de la interrupción UART0
    if((iterrupcion & 0x0e) == 4){                  //Si la interrupción se ha generado por Receive Data Available(RDA)
        callback_linea_serie_drv_leer(U0RBR);
    }else if((iterrupcion & 0x0e)==2){
        U0IER = 0x1;
        callback_linea_serie_drv_escribir();        //Si la interrupción se ha generado por Transmit Holding Register Empty(THRE)
    }
    VICVectAddr = 0;
}

//función que estable la configuración inicial de la linea serie, 
// configura los pines y habilita las interrupciones
void linea_serie_hal_inicializar(void (*callback_linea_serie_drv_leer_param)(), void (*callback_linea_serie_drv_continuar_envio_param)()){
    callback_linea_serie_drv_leer = callback_linea_serie_drv_leer_param;
    callback_linea_serie_drv_continuar_envio = callback_linea_serie_drv_continuar_envio_param;
    PINSEL0 |= 0x5;                                     //Configura la función del pin P1.0 como UART0                                         
    U0LCR = 0x80;
    U0DLL = 91;                                         //falta
    U0LCR = 3;
    U0IER = 0x3;
    VICVectCntl4 = 0x20 | 6;                            //Configura la interrupcion, 0x20 habilita las interrupciones vectorizadas y 6 es el numero de interrupción UART0
    VICVectAddr4 = (unsigned long)linea_serie_ISR;      //Establece la dirección de la rutina de interrupción para UART0 en el vector de interrupciones
    VICIntEnable |= 0x00000040;                         //Habilita las interrupciones UART0

}

//Se activan las interrupciones THRE y se asigna al registro de transmision el caracter
void linea_serie_hal_escribir(uint8_t caracter){
    U0IER |= 0x2;
    U0THR = caracter;

}

