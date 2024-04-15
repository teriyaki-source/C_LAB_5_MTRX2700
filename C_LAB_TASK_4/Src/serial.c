#include "serial.h"
#include "flags.h"
#include "stm32f303xc.h"
#include "config.h"
#include <string.h>

// We store the pointers to the GPIO and USART that are used
//  for a specific serial port. To add another serial port
//  you need to select the appropriate values.
struct _SerialPort {
	USART_TypeDef *UART;
	GPIO_TypeDef *GPIO;
	volatile uint32_t MaskAPB2ENR;	// mask to enable RCC APB2 bus registers
	volatile uint32_t MaskAPB1ENR;	// mask to enable RCC APB1 bus registers
	volatile uint32_t MaskAHBENR;	// mask to enable RCC AHB bus registers
	volatile uint32_t SerialPinModeValue;
	volatile uint32_t SerialPinSpeedValue;
	volatile uint32_t SerialPinAlternatePinValueLow;
	volatile uint32_t SerialPinAlternatePinValueHigh;
	void (*completion_function)(uint32_t);
};


// instantiate the serial port parameters
//   note: the complexity is hidden in the c file
SerialPort USART1_PORT = {USART1,
		GPIOC,
		RCC_APB2ENR_USART1EN, // bit to enable for APB2 bus
		0x00,	// bit to enable for APB1 bus
		RCC_AHBENR_GPIOCEN, // bit to enable for AHB bus
		0xA00,
		0xF00,
		0x770000,  // for USART1 PC10 and 11, this is in the AFR low register
		0x00, // no change to the high alternate function register
		0x00 // default function pointer is NULL
		};

// InitialiseSerial - Initialise the serial port
// Input: baudRate is from an enumerated set
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t)) {

	serial_port->completion_function = completion_function;

	// enable clock power, system configuration clock and GPIOC
	// common to all UARTs
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// enable the GPIO which is on the AHB bus
	RCC->AHBENR |= serial_port->MaskAHBENR;

	// set pin mode to alternate function for the specific GPIO pins
	serial_port->GPIO->MODER = serial_port->SerialPinModeValue;

	// enable high speed clock for specific GPIO pins
	serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;

	// set alternate function to enable USART to external pins
	serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;

	// enable the device based on the bits defined in the serial port definition
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;

	// Get a pointer to the 16 bits of the BRR register that we want to change
	uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR; // only 16 bits used!

	// Baud rate calculation from datasheet
	switch(baudRate){
	case BAUD_9600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_19200:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_38400:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_57600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_115200:
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	}

	// enable serial port for tx and rx
	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

/*
||------------------||
||	UART Interrupt	||
||------------------||
*/

uint8_t *buffer[BUFFER_SIZE] = {0};
uint8_t *last_word[BUFFER_SIZE] = {0};
uint8_t *action;
int i = 0;

void (*on_key_input)() = 0x00;

void getChar(SerialPort *serial_port, uint8_t* buffer, uint8_t* last_word, int* i) {
	// gets the current count in the word
	uint8_t x = *i;
	// checks the flags of the input port - clears them if raised
	if ((serial_port->UART->ISR & USART_ISR_ORE) != 0 || (serial_port->UART->ISR & USART_ISR_FE) != 0){
		serial_port->UART->ICR |= USART_ICR_ORECF;
		serial_port->UART->ICR |= USART_ICR_FECF;
	}
	// if the receive not empty flag is not 0 i.e. there is a character ready to be received
	if ((serial_port->UART->ISR & USART_ISR_RXNE) != 0){
		// get the value from the Read Data Register (RDR)
		buffer[x] = serial_port->UART->RDR;
		// increment the counter for the position in the word
		*i += 1;
		if (buffer[x] == TERMINATION_CHAR) {
			// add a newline to the buffer for better output aesthetic
			buffer[x+1] = NEWLINE_CHAR;
			// can be removed - demonstrate double buffer
			SerialOutputString(buffer, &USART1_PORT);
//			SerialOutputString(last_word, &USART1_PORT);
			// any additional handling should go here - before buffer is cleared

			serial_port->completion_function(buffer);

			// copy word to previous word and then clear current buffer
			strncpy(last_word, buffer, BUFFER_SIZE);
			for (int j = 0; j < BUFFER_SIZE; j++){
				buffer[j] = 0;
			}
			// reset the counter to 0
			*i = 0;

		}
	}
}
void enable_uart_interrupt(SerialPort *serial_port){
	__disable_irq();

	// enable the read data not empty interrupt enable bit in the control register
	serial_port->UART->CR1 |= USART_CR1_RXNEIE;

	NVIC_SetPriority(USART1_IRQn, 1);
	NVIC_EnableIRQ(USART1_IRQn);

	// set the interrupt function
	on_key_input = &getChar;

	__enable_irq();
}

void USART1_EXTI25_IRQHandler(){
	// should receive a character and store it in a buffer then return
	on_key_input(&USART1_PORT, buffer, last_word, &i);
}

/*
||------------------||
||	  UART Output	||
||------------------||
*/

void SerialOutputChar(uint8_t data, SerialPort *serial_port) {

	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){
	}
	serial_port->UART->TDR = data;
}


void SerialOutputString(uint8_t *pt, SerialPort *serial_port) {
	uint32_t counter = 0;
	while(*pt) {
		SerialOutputChar(*pt, serial_port);
		counter++;
		pt++;
	}
}

/*
||------------------------------||
||	  UART Completion Function	||
||------------------------------||
*/

void USART_callback(uint8_t *string) {
// This function will be called after a transmission is complete
	// use a temporary var to avoid original buffer from being modified
	uint8_t *temp[BUFFER_SIZE] = {0};
	uint8_t *instructions[2] = {0};
	int i = 0;
	strncpy(temp, string, BUFFER_SIZE);

	uint8_t *token = strtok(temp, " ");
	instructions[i++] = token;
	token = strtok(NULL, "");
	instructions[i++] = token;


	message_handler(instructions[0], instructions[1]);
}
