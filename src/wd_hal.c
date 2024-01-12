

#include "wd_hal.h"

//inicializar el watchdog timer para que resetee el procesador dentro de sec segundos si no se "alimenta"
void WD_hal_inicializar(int sec){
    //configuracion del watchdog
    WDTC = sec * 15000000/4; // 1500 * 1000 ; 1000ms = 1s
    WDMOD = 0x03;
}

//alimentar el watchdog
void WD_hal_feed(){
    uint32_t irq;
    irq = read_IRQ_bit();       //guardamos el estado de las interrupciones
    //deshabilita las interrupciones unicamente si las interrupciones estan habilitadas
    if(irq == 0){
       disable_irq();
    }   
    //alimentamos el watchdog
    WDFEED = 0xAA;            
    WDFEED = 0X55;          
    //restablece el estado de las interrupciones
    if(irq == 0){
        enable_irq();
    }       
        
    
    

}

//test con un bucle infinito y compurbear que se resetea y por tanto que el watchdog funciona
void WD_hal_test(){
    while(1){}      //bucle infinito 
}
