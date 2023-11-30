#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H
#include <inttypes.h>
#include "hello_world.h"
//#include "power.h"
#include "alarmas.h"
#include "botones.h"
#include "visualizar.h"
#include "linea_serie_drv.h"
#include "fifo.h"
#include "io_reserva.h"

#include "juego.h"


#define USUARIO_AUSENTE 12000


void planificador(void);

#endif
