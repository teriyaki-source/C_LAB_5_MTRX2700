#include "button.h"
#include "timer.h"

uint8_t led_flag;

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

	// Re-enable interrupts
	__enable_irq();
}

void get_current_led(uint8_t *bitmask)
{
	*bitmask = *(((uint8_t*)&(GPIOE->ODR)) + 1);
}

void store_led(uint8_t bitmask)
{
	uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	*led_register = bitmask;
}

void led_flag_on()
{
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

	//if the leds all on, resets everything
	if(bitmask == 0b11111111)
	{
		bitmask = 0b00000000;
	}

	//otherwise turn one more on
	else
	{
		bitmask <<= 1;
		bitmask += 1;
	}

	//store led bitmask back
	store_led(bitmask);

}
