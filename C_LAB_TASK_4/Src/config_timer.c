#include <stdint.h>
#include "config_timer.h"
#include "flags.h"
#include "stm32f303xc.h"




// Define callback functions for various interrupts
void (*tim3_overflow_callback)() = 0x00;
void (*tim2_ccr1_callback)() = 0x00;


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
	// Disable the interrupts while messing around with the settings
	__disable_irq();

	// Make timer 2 trigger an interrupt when there is
	// a successful output compare (on channel 1)
	TIM2->DIER |= TIM_DIER_CC1IE;
	// We use the capture/compare1 interrupt enable TIM_DIER_CC1IE
	// Setting this bit in the DMA and interrupt enable register (DIER)of TIM2
	// This configures the timer to generate an interrupt when the timers counter (CNT)
	// When CNT matchest the same value as CCR1

	// Tell the NVIC module that timer 2 interrupts should be handled
	NVIC_SetPriority(TIM2_IRQn, 2);  // set priority
	NVIC_EnableIRQ(TIM2_IRQn);

	// Re-enable all interrupts (now that we are finished)
	__enable_irq();
}


// Enable interrupts for overflow and capture/compare 1 in timer 3
void config_tim3_interrupts()
{
	// Disable the interrupts while messing around with the settings
	__disable_irq();
	// Activate the update interrupt (UIE) for Tim3
	// Make the timer 3 trigger an interrupt when there is an overflow
	TIM3->DIER |= TIM_DIER_UIE;
	// Inherently periodic since it occurs every time
	// That the timer completes counting up to the reload value

	// Tell the NVIC module that timer 3 interrupts should be handled
	NVIC_SetPriority(TIM3_IRQn, 3);  // set priority
	NVIC_EnableIRQ(TIM3_IRQn);

	// Re-enable all interrupts (now that we are finished)
	__enable_irq();
}


void config_button_interrupts()
{
	// Disable the interrupts while messing around with the settings
	__disable_irq();

	// Enable the system configuration controller (SYSCFG in RCC)
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// External Interrupts details on large manual page 294
	// PA0 is on interrupt EXTI0 large manual - page 250
	// EXTI0 in SYSCFG_EXTICR1 needs to be 0x00 (SYSCFG_EXTICR1_EXTI0_PA)
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;

	// Select EXTI0 interrupt on rising edge
	EXTI->RTSR |= EXTI_RTSR_TR0;  // rising edge of EXTI line 0 (includes PA0)

	// set the interrupt from EXTI line 0 as 'not masked' - as in, enable it.
	EXTI->IMR |= EXTI_IMR_MR0;

	// Tell the NVIC module that EXTI0 interrupts should be handled
	NVIC_SetPriority(EXTI0_IRQn, 1);  // Set Priority
	NVIC_EnableIRQ(EXTI0_IRQn);

	// Re-enable all interrupts (now that we are finished)
	__enable_irq();
}


// Toggles the LEDs
void toggle_led()
{
	uint8_t *led_register = ((uint8_t *)&(GPIOE->ODR)) + 1;
	*led_register ^= 0b11111111;
}


void set_compare_channel_1(TIM_TypeDef *TIM, uint32_t compare_value)
{
	TIM->CCR1 = compare_value;
}


void set_autoreload(TIM_TypeDef *TIM, uint32_t autoreload_value)
{
	TIM->ARR = autoreload_value;
}

void trigger_prescaler(TIM_TypeDef *TIM) {
	__disable_irq();
	TIM->ARR = 0x01;
	TIM->CNT = 0x00;
	asm("NOP");
	asm("NOP");
	asm("NOP");
	TIM->ARR = 0xffffffff;
	__enable_irq();
}

// Function to set the prescaler value for a specified timer
void set_prescaler(TIM_TypeDef *TIM, uint16_t prescaler)
{
    // Set the prescaler value for the timer, defining how the timer's clock is divided
    TIM->PSC = prescaler;
	trigger_prescaler(TIM);
    // Trigger an update event to apply the new prescaler immediately and reset the timer counter
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


// Chase LEDs cyclicly such that the value of the last LED becomes
// the value of the first LED
// Assumes that at least one LED is already on
void chase_led()
{
    // Access the higher byte of the GPIOE output data register (ODR)
    uint8_t *led_register = ((uint8_t *)&(GPIOE->ODR)) + 1;

    // Isolate the highest bit (LED) of the register, then shift right to make it the LSB
    uint8_t last_led = (*led_register & 0b10000000) >> 7;  // 1 if last LED is on, else 0

    // Shift all bits in the register to the left, moving each LED state to the next LED
    *led_register <<= 1;

    // Set the LSB to the value of the previously last LED (now wrapped around)
    *led_register |= last_led;
}


// Setup a one-shot timer, trigger LED toggle, and plan return to chase
// Function to set up a one-shot timer event with a delay and a callback function
void setup_one_shot_timer(uint16_t delay_ms, void (*one_shot_callback)()) {
    // Set TIM2's compare register (CCR1) to the current count plus delay, wrapped by max count
    TIM2->CCR1 = (TIM2->CNT + delay_ms) % TIM2_MAX_COUNT;

    // Assign the callback to be triggered when TIM2's count matches CCR1
    tim2_ccr1_callback = one_shot_callback;
}


void trigger_led_inversion() {
    setup_one_shot_timer(ONE_SHOT_DELAY_MS, &toggle_leds_and_start_timer);
}

void toggle_leds_and_start_timer() {
    toggle_led();  // Assume this toggles to the inverted state
    setup_one_shot_timer(2800  , &resume_led_chase);  // Continue in inverted state for 2800ms
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

    // Set the prescaler if necessary
    TIM3->PSC = PRESCALER;  // Prescaler setup, adjust as necessary

    // Set the auto-reload value to the current timer period
    TIM3->ARR = timer_period - 1;  // ARR value is period-1

    // Generate an update event to load the prescaler and the ARR value immediately
    TIM3->EGR = TIM_EGR_UG;

    // Re-enable Timer 3
    TIM3->CR1 |= TIM_CR1_CEN;
}

void set_timer_period(uint16_t new_period) {
    timer_period = new_period;  // Update the static variable holding the period
    configure_timer();  // Apply the new configuration
}


void timer_start_up(){
	set_prescaler(TIM2, PRESCALER);  // 1 count = 1 millisecond
	set_prescaler(TIM3, PRESCALER);  // 1 count = 1 millisecond

	config_tim2_interrupts();  // timer 2 is used for the one-shot delay
	config_tim3_interrupts();  // timer 3 is used for the periodic callback
}

uint16_t str_to_time(uint8_t* original){

	char *ptr;

	uint16_t time = strtol(original, &ptr, 10);

	return(time);
}

void oneshot_command(uint8_t *oneshot_length){
	//uint16_t delay_ms, void (*one_shot_callback
	uint16_t delay_ms = str_to_time(oneshot_length);
	// Start the timer 2 counter
	TIM2->CR1 |= TIM_CR1_CEN;

	setup_one_shot_timer(delay_ms, set_oneshot_flag);
}

void timer_command(uint8_t *timer_interval){
	// Change the timer period based on a condition or a new requirement
	TIM3->CNT = 0x00;
	uint16_t new_period = str_to_time(timer_interval);

	set_timer_period(new_period);

	// Set the function pointers used in the interrupt handlers
	// to their corresponding callback functions
	tim3_overflow_callback = set_continuous_flag;

	// Start the timer 3 counter
	TIM3->CR1 |= TIM_CR1_CEN;
}
