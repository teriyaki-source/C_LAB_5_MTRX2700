#include "led.h"
#include "serial.h"
#include <stdlib.h>

void get_current_led(uint8_t *bitmask)
{
	*bitmask = *(((uint8_t*)&(GPIOE->ODR)) + 1);
}

void store_led(uint8_t bitmask)
{
	uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	*led_register = bitmask;
}

uint8_t str_to_bin(uint8_t* original){
	char* ptr;

	uint8_t target = strtol(original, &ptr, 2);
	return(target);

}

void modify_led(uint8_t *input)
{
	//variable for storing bitmask
	uint8_t temp[MASK_LENGTH] = {0};
	uint8_t new_mask = 0b00000000;
	uint8_t bitmask;

	// remove trailing /r/n previously added for printability
	strncpy(temp, input, MASK_LENGTH);

	// convert uint8_t array of chars into singular char
	new_mask = str_to_bin(temp);

	//get the current bitmask for led
	get_current_led(&bitmask);

	//if the leds all on, resets everything
	bitmask = new_mask;

	//store led bitmask back
	store_led(bitmask);

}


