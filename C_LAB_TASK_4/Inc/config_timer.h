#ifndef CONFIG_TIMER_H_
#define CONFIG_TIMER_H_

#include <stdint.h>
#include "stm32f303xc.h"
//Constants
#define PRESCALER 7999  // value to put into prescaler register such that 1 count is 1 ms
#define INTERVAL_MS 200 // interval (ms) between function callbacK
#define ONE_SHOT_DURATION 2800
#define ONE_SHOT_DELAY_MS 1000  // delay between pressing button and callback being called
#define TIM2_MAX_COUNT 0xFFFFFFFF  // default auto-reload value for timer 2


// Enable the clocks for desired peripherals
void enable_clocks();

// Initialise the discovery board I/O (just outputs: inputs are selected by default)
void initialise_board();

void config_tim2_interrupts();

// Enable interrupts for overflow and capture/compare 1 in timer 3
void config_tim3_interrupts();

void config_button_interrupts();

void set_compare_channel_1(TIM_TypeDef *TIM, uint32_t compare_value);

void set_autoreload(TIM_TypeDef *TIM, uint32_t autoreload_value);

void set_prescaler(TIM_TypeDef *TIM, uint16_t prescaler);

void turn_on_led(uint8_t led_mask_pattern);

// Toggles the LEDs
void toggle_led();

// Chase LEDs cyclicly such that the value of the last LED becomes
// the value of the first LED
// Assumes that at least one LED is already on
void chase_led();

// Triggers a callback function after a certain delay
void setup_one_shot_timer(uint16_t delay_ms, void (*one_shot_callback)());

void trigger_led_inversion();

void toggle_leds_and_start_timer();

void resume_led_chase();

// Function to configure or reset Timer 3 hardware with a new period
static void configure_timer();

void set_timer_period(uint16_t new_period);
void timer_start_up();
uint16_t str_to_time(uint8_t* original);
void oneshot_command(uint8_t *oneshot_length);
void timer_command(uint8_t *timer_interval);



#endif /* CONFIG_H_ */
