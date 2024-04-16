#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include "stm32f303xc.h"

#define NOT_PRESSED 0
#define PRESSED 1

//interrupt handler for button
void EXTI0_IRQHandler();
//enable interrupt for button (PA0)
void enable_interrupt_button();
//get the current status of led
void get_current_led(uint8_t *bitmask);
//store the led bitmask back to led register
void store_led(uint8_t bitmask);
void led_flag_on();
void led_flag_off();
//function to modify the led
void modify_led();
//function to call the start up function for this module
void led_start_up();
//function to check the flag and handle it
void flag_check();

#endif
