#ifndef LED_H_
#define LED_H_

#include <stdint.h>
#include "stm32f303xc.h"
#include "serial.h"

#define NOT_PRESSED 0
#define PRESSED 1

#define MASK_LENGTH 8
#define OB_OFFSET 2

//get the current status of led
void get_current_led(uint8_t *bitmask);
//store the led bitmask back to led register
void store_led(uint8_t bitmask);
void led_flag_on();
void led_flag_off();
//function to modify the led
uint8_t str_to_bin(uint8_t* original);

void modify_led(uint8_t *new_mask);

#endif /* LED_H_ */
