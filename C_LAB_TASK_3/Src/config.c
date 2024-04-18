#include <stdint.h>
#include "config.h"
#include "stm32f303xc.h"




extern void (*tim3_overflow_callback)();
extern void (*tim2_ccr1_callback)();




// Enable the clocks for desired peripherals
void enable_clocks()
{
	// Enable the clocks for GPIOA, C, and E
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;
	// Enable the clocks for timer 2 and timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;
}


// Initialise the discovery board I/O (just outputs: inputs are selected by default)
void initialise_board()
{
	// Get a pointer to the second half word of the MODER register (for outputs pe8-15)
	uint16_t *led_mode_register = ((uint16_t *)&(GPIOE->MODER)) + 1;
	*led_mode_register = 0x5555;
}


void config_tim2_interrupts()
{
	__disable_irq();

	// Make timer 2 trigger an interrupt when there is
	// a successful output compare (on channel 1)
	TIM2->DIER |= TIM_DIER_CC1IE;
	// We use the capture/compare1 
	// interrupt enable TIM_DIER_CC1IE
	// Setting this bit in the DMA and interrupt 
	// enable register (DIER) of TIM2
	// This configures the timer to generate an interrupt when the timers counter (CNT)
	// When CNT matchest the same value as CCR1

	// Tell the NVIC module tim2's priority
	NVIC_SetPriority(TIM2_IRQn, 2);  
	NVIC_EnableIRQ(TIM2_IRQn);

	// Re-enable all interrupts
	__enable_irq();
}


// Enable interrupts for overflow and capture/compare 1 in timer 3
void config_tim3_interrupts()
{
	__disable_irq();
	// Activate the update interrupt (UIE) for Tim3
	// Make the timer 3 trigger an interrupt when there is an overflow
	// Periodic since it occurs every time the timer completes 
	// counting up to the reload value and resets
	TIM3->DIER |= TIM_DIER_UIE;
	

	// Tell the NVIC module tim3's priority
	NVIC_SetPriority(TIM3_IRQn, 3);  
	NVIC_EnableIRQ(TIM3_IRQn);

	// Re-enable all interrupts 
	__enable_irq();
}


void config_button_interrupts()
{
	__disable_irq();

	// Enable the system configuration controller (SYSCFG in RCC)
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;
	// External Interrupts details on large manual page 294
	// PA0 is on interrupt EXTI0 large manual - page 250
	// EXTI0 in SYSCFG_EXTICR1 needs to be 0x00 (SYSCFG_EXTICR1_EXTI0_PA)

	// Select EXTI0 interrupt on rising edge
	EXTI->RTSR |= EXTI_RTSR_TR0;  // rising edge of EXTI line 0 (includes PA0)

	// set the interrupt from EXTI line 0 as 'not masked' 
	EXTI->IMR |= EXTI_IMR_MR0;

	// Tell the NVIC module button presses's priority
	NVIC_SetPriority(EXTI0_IRQn, 1);  
	NVIC_EnableIRQ(EXTI0_IRQn);

	// Re-enable all interrupts 
	__enable_irq();
}


// Toggles the LEDs connected to the upper half of GPIOE
void toggle_led()
{
    // Obtain a pointer to the byte (8 bits) in the ODR (Output Data Register) of GPIOE
    // that corresponds to the LED pins. This is assuming that LEDs are connected to
    // pins 8-15 of GPIOE, which are represented by the second byte of the ODR.
    uint8_t *led_register = ((uint8_t *)&(GPIOE->ODR)) + 1;
    
    // Toggle all LEDs by performing a bitwise XOR on the register's current value with
    // a byte where all bits are set to 1 (0b11111111). This will invert the state of each LED.
    *led_register ^= 0b11111111;
}

// Sets the compare match value for the Capture/Compare Register 1 of a given timer
void set_compare_channel_1(TIM_TypeDef *TIM, uint32_t compare_value)
{
    // Assign the provided compare_value to the CCR1 (Capture/Compare Register 1) of the specified timer.
    // When the timer's counter matches this value, a capture/compare interrupt can be generated if enabled.
    TIM->CCR1 = compare_value;
}

// Sets the auto-reload value for the specified timer
void set_autoreload(TIM_TypeDef *TIM, uint32_t autoreload_value)
{
    // Load the provided autoreload_value into the ARR (Auto-Reload Register) of the specified timer.
    // The timer will count up to this value and then reset to 0, which can trigger an overflow interrupt if enabled.
    TIM->ARR = autoreload_value;
}

// Sets the prescaler value for a specified timer
void set_prescaler(TIM_TypeDef *TIM, uint16_t prescaler)
{
    // Set the prescaler value for the timer, defining how the timer's clock is divided.
    // The prescaler slows down the timer's counting rate: a prescaler of N reduces the
    // clock frequency by a factor of N+1 (e.g., a prescaler of 7999 results in a frequency division by 8000).
    TIM->PSC = prescaler;

    // Trigger an update event to apply the new prescaler value immediately.
    // This ensures that the timer's counter is updated with the new prescaler without any delay.
    // The UG bit in the EGR (Event Generation Register) is set to reinitialize the counter
    // and update the prescaler registers.
    TIM->EGR |= TIM_EGR_UG;
}



// Function to set the LED states for a group of LEDs connected to the upper half of GPIOE
void turn_on_led(uint8_t led_mask_pattern)
{
    // Access the upper byte of the GPIOE output data register directly
    uint8_t *led_register = ((uint8_t *)&(GPIOE->ODR)) + 1;

    // Set the LED states according to the specified mask pattern
    *led_register = led_mask_pattern;
}


// Rotates the LED pattern where the end becomes the start.
void chase_led()
{
    // Point to the high byte of GPIOE's output data register for the upper LEDs.
    uint8_t *led_register = ((uint8_t *)&(GPIOE->ODR)) + 1;

    // Save the state of the last LED (bit 7).
    uint8_t last_led = (*led_register & 0b10000000) >> 7;

    // Rotate all LED states left, moving each to the next position.
    *led_register <<= 1;

    // Place the last LED state in the first position.
    *led_register |= last_led;
}

// Configures a one-shot timer event with a delay and callback.
void setup_one_shot_timer(uint16_t delay_ms, void (*one_shot_callback)()) {
    // Set timer's compare value for the delay and assign the callback.
    TIM2->CCR1 = (TIM2->CNT + delay_ms) % TIM2_MAX_COUNT;
    tim2_ccr1_callback = one_shot_callback;
}

// Initiates LED inversion after a predefined delay.
void trigger_led_inversion() {
    // Use the one-shot timer to toggle LEDs after a delay.
    setup_one_shot_timer(ONE_SHOT_DELAY_MS, &toggle_leds_and_start_timer);
}

}

void toggle_leds_and_start_timer() {
    toggle_led();  // Assume this toggles to the inverted state
    setup_one_shot_timer(3200  , &resume_led_chase);  // Continue in inverted state for 3200ms
	//for 2 cycles of the leds 
}

void resume_led_chase() {
    toggle_led();  // Toggle back to normal state if needed
    tim3_overflow_callback = &chase_led;  // Ensure the regular LED chase resumes
}

static uint32_t timer_period = INTERVAL_MS;  // Initial default period

// Function to configure or reset Timer 3 hardware with a new period
static void configure_timer() {
    // Disable Timer 3 while updating settings to avoid glitches
    TIM3->CR1 &= ~TIM_CR1_CEN;

    /
    TIM3->PSC = PRESCALER - 1;  // Prescaler setup

    // Set the auto-reload value to the current timer period
    TIM3->ARR = timer_period - 1;  // ARR value is period-1

    // Generate an update event to load the prescaler and the ARR value immediately
    TIM3->EGR = TIM_EGR_UG;

    // Re-enable Timer 3
    TIM3->CR1 |= TIM_CR1_CEN;
}

void set_timer_period(uint32_t new_period) {
    timer_period = new_period;  // Update the static variable holding the period
    configure_timer();  // Apply the new configuration
}


