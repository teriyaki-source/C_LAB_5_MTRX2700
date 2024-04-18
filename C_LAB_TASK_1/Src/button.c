#include "button.h"
#include "timer.h"

uint8_t led_flag;

//function pointer to a function when a button is pressed, set to NULL so when it not assigned to any
//specific function it wont be called
void (*button_pressed)() = 0x00;

void EXTI0_IRQHandler()
{
	//call function button_pressed if its not NULL pointer
	if (button_pressed != 0x00)
	{
		button_pressed();
	}
	EXTI->PR |= EXTI_PR_PR0;
}

void enable_interrupt_button()
{
	// disable interrupt
	__disable_irq();

	// enable the system configuration controller
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// External Interrupts PA0
	// EXTI0 in SYSCFG_EXTICR1 needs to be 0x00
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;

	//  Select EXTI0 interrupt on rising edge
	EXTI->RTSR |= EXTI_RTSR_TR0; // rising edge of EXTI line 0 (includes PA0)

	// set the interrupt from EXTI line 0 as 'not masked' - as in, enable it.
	EXTI->IMR |= EXTI_IMR_MR0;

	// Tell the NVIC module that EXTI0 interrupts should be handled
	NVIC_SetPriority(EXTI0_IRQn, 1);  // Set Priority
	NVIC_EnableIRQ(EXTI0_IRQn);

	//set handler function
	button_pressed = led_flag_on;

	// Re-enable interrupts
	__enable_irq();
}

void get_current_led(uint8_t *bitmask)
{
	//get current bitmask from the LED register
	*bitmask = *(((uint8_t*)&(GPIOE->ODR)) + 1);
}

void store_led(uint8_t bitmask)
{
	//get the address of the led register
	uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	//store the bitmask back to the register
	*led_register = bitmask;
}

void led_flag_on()
{
	//set flag to PRESSED
	led_flag = PRESSED;
}

void led_flag_off()
{
	//reset the flag to NOT_PRESSED
	led_flag = NOT_PRESSED;
}

void modify_led()
{
	//variable for storing bitmask

	uint8_t bitmask;

	//get the current bitmask for led
	get_current_led(&bitmask);


	//shift the LED by 1 place
	bitmask <<= 1;
	if (bitmask == 0)
	{
		bitmask = 1;
	}

	//store led bitmask back
	store_led(bitmask);

}


void led_start_up()
{
	//call to functions for setting up the led
	enable_clocks();

	initialise_board();

	enable_interrupt_button();
}

void flag_check()
{
	//checking the status of the flag
	if(led_flag == PRESSED)
	{
		//if flag raised:
		//call function to modify LEDs
		modify_led();
		//call function to limit the speed the LED can be changed
		timer_loop();
		//clear the flag.
		led_flag_off();
	}
}

