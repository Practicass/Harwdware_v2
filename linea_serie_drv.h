#ifndef LINEA_SERIE_DRV_H
#define LINEA_SERIE_DRV_H

#include "linea_serie_hal.h"

//#include "gpio_hal.h"
//#include "SWI.h"


//inicializa las estructuras de datos, inicializa linea_serie_hal y asigna los valores de los pines y de los eventos
void linea_serie_drv_inicializar(void (*callback_fifo_encolar_param)(), uint8_t id_ev_RX_SERIE, uint8_t id_ev_TX_SERIE, void (*callback_gpio_hal_sentido_param)(), void (*callback_gpio_hal_escribir_param)(), uint32_t (*callback_gpio_hal_leer_param)(),int GPIO_SERIE_ERROR_PARAM, int GPIO_SERIE_ERROR_BITS_PARAM, int GPIO_HAL_PIN_DIR_OUTPUT_PARAM);

//lee el caracter introducido por el usuario y lo almacena en el buffer de entrada, comprueba que el caracter 
//introducido es de formato correcto y genera el evento ev_RX_SERIE
void linea_serie_drv_leer(uint8_t caracter);

//inicializa el buffer de salida y envia el primer caracter
void linea_serie_drv_enviar_array(uint8_t cadena[]);

//envia el siguiente caracter del buffer de salida, si se ha enviado el ultimo caracter se genera el evento ev_TX_SERIE
void linea_serie_drv_continuar_envio(void);

//envia el caracter introducido por el usuario a la linea serie
void linea_serie_drv_escribir(uint8_t caracter);

#endif

