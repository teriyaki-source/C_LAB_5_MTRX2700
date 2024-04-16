#include <stdint.h>
#include "config_timer.h"
#include "interrupts.h"
#include "stm32f303xc.h"
#include "serial.h"


// Define callback functions for various interrupts
extern void (*tim3_overflow_callback)();
extern void (*tim2_ccr1_callback)();
//extern void (*button_press_callback)(uint16_t delay_ms, void (*one_shot_callback));
uint8_t *ONESHOT_MESSAGE = "Oneshot timer finished\r\n";
uint8_t *TIMER_MESSAGE = "Timer period finished\r\n";

// Handle interrupts for timer 2 (one-shot callback)
void TIM2_IRQHandler()
{
    // Check if the interrupt was due to the channel 1 compare flag
    if ((TIM2->SR & TIM_SR_CC1IF) != 0)
    {
    	//check if a callback function is registered
        if (tim2_ccr1_callback != 0x00)
        {
            tim2_ccr1_callback(ONESHOT_MESSAGE, &USART1_PORT);  // Execute the registered callback function
        }

        // Clear the channel 1 compare flag to prCzxevent re-entering
        TIM2->SR &= ~TIM_SR_CC1IF;
    }
}



// Handle interrupts for timer 3 (periodic callback)
void TIM3_IRQHandler()
{
	// Branch based on the flag
	// The status register tells us why the interrupt was called
	// UIF is from an overflow type event
	if ((TIM3->SR & TIM_SR_UIF) != 0) //check if the interrupt was triggered by an overflow event
	{
		//check if the callback function is registered for overflow events
		if (tim3_overflow_callback != 0x00)
		{
			// Trigger callback function
			tim3_overflow_callback(TIMER_MESSAGE, &USART1_PORT); //trigger the overflow callback fucntion
		}

		// Reset overflow flag to allow new overflow events to be detected rather than re-entering
		TIM3->SR &= ~TIM_SR_UIF;
	}
}
