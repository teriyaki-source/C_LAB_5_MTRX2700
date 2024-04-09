#ifndef TIMER_H
#define TIMER_H

#include "stm32f303xc.h"
#include <stdint.h>

//set prescaler for timer
void trigger_prescaler();
//start the timer running
void start_timer();
//timer loop
void timer_loop(uint32_t time_length);

#endif
