#ifndef INITIALISATION_H
#define INITIALISATION_H

#include <stdint.h>
#include "stm32f303xc.h"

//enable the clocks for GPIOA, C and E and clock for timer
void enable_clocks();
//initialise the discovery board I/O, output for leds on
void initialise_board();


#endif
