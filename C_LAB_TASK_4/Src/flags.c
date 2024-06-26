#include "flags.h"
#include "stm32f303xc.h"
#include "config.h"
#include "led.h"
#include "serial.h"
#include "config_timer.h"

struct _Flags {
	int led_flag;
	uint8_t* led_action;
	int serial_flag;
	uint8_t* serial_action;
	int oneshot_flag;
	uint8_t* oneshot_action;
	int timer_flag;
	uint8_t* timer_action;
};

Flags flags = {
	OFF,
	OFF,
	OFF,
	OFF,
	OFF,
	OFF,
	OFF,
	OFF,
};

struct _Timer_Flags{
	uint8_t oneshot_flag;
	uint8_t continuous_flag;
};

Timer_flags timer_flags = {
	OFF,
	OFF,
};


void message_handler(uint8_t* instruction, uint8_t *action){
	// still operating in interrupt
	if (!strcmp(instruction, "led")) {
		flags.led_flag = ON;
		flags.led_action = action;
	}
	if (!strcmp(instruction, "serial")) {
		flags.serial_flag = ON;
		flags.serial_action = action;
	}
	if (!strcmp(instruction, "oneshot")) {
		flags.oneshot_flag = ON;
		flags.oneshot_action = action;
	}
	if (!strcmp(instruction, "timer")) {
		flags.timer_flag = ON;
		flags.timer_action = action;
	}
}

void set_oneshot_flag(){
	timer_flags.oneshot_flag = ON;
}

void set_continuous_flag(){
	timer_flags.continuous_flag = ON;
}

void timer_handler(){
	uint8_t *oneshot_message = "Oneshot finished\r\n";
	uint8_t *continuous_message = "Timer period finished\r\n";

	if(timer_flags.oneshot_flag)
		SerialOutputString(oneshot_message, &USART1_PORT);
		timer_flags.oneshot_flag = OFF;
	if(timer_flags.continuous_flag)
		SerialOutputString(continuous_message, &USART1_PORT);
		timer_flags.continuous_flag = OFF;
}

void flag_handler(){
	// not operating in interrupt
	timer_handler();
	// action based on flag, needs to reset flag after action complete
	if (flags.led_flag){
		// set LED bits
//		SerialOutputString(flags.led_action, &USART1_PORT);
		modify_led(flags.led_action);
		flags.led_flag = OFF;
	}
	if (flags.serial_flag){
		// send output message
		SerialOutputString(flags.serial_action, &USART1_PORT);
		flags.serial_flag = OFF;
	}
	if (flags.oneshot_flag){
		// start a oneshot timer with set period
		//SerialOutputString(flags.oneshot_action, &USART1_PORT);
		oneshot_command(flags.oneshot_action);
		flags.oneshot_flag = OFF;
	}
	if (flags.timer_flag){
		if(!strcmp(flags.timer_action, "stop\r\n")) {
			TIM3->CR1 &= ~TIM_CR1_CEN;
		}
		else{
		// start a repeated timer with set period
		//SerialOutputString(flags.timer_action, &USART1_PORT);
			timer_command(flags.timer_action);
		}
		flags.timer_flag = OFF;
		timer_flags.continuous_flag = OFF;
	}
}


