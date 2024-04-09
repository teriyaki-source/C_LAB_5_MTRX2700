#ifndef BUTTON_H
#define BUTTON_h

#include <stdint.h>
#include "stm32f303xc.h"

//enable interrupt for button (PA0)
void enable_interrupt_button();
//get the current status of led
void get_current_led(uint8_t *led_bitmask);
//store the led bitmask back to led register
void store_led(uint8_t led_bitmask);
//function to modify the led
void modify_led();


#endif
