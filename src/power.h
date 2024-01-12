
#include <LPC210X.H>

extern void Switch_to_PLL(void);

//reduce el consumo de energia del microprocesador
void power_hal_wait(void);

//dueerme el microprocesador por completo
void power_hal_deep_sleep(void);

