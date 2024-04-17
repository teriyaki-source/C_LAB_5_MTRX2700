# MTRX2700_C_LAB_5
C Lab for MTRX2700 2024 Sem 1
## Group Members:
Luke Marinaki, Matthew Ma, Yunkan Luo
## Roles and Responsibilities
Task 1: Yunkan Luo\
Task 2: Luke Marinaki\
Task 3: Matthew Ma\
Task 4: All together
## Project Overview
### Task 1:
Author: Yunkan Luo
#### Exercises:
a,b,c): Using the user input button on the board and treat it as an interrupt to change the pattern of LEDs.
d):     Restrict the speed the pattern can be changed
#### Instruction:
* Press the user input button and the LEDs would be changed.
* The time between two changes is set to be 1 second by default setting.
* The first time pressing the button would light up one LED, and any presses after that will shift the LED to the nearby location.
* DO NOT make changes to any values inside the functions and definitions other than 'DELAY' inside "timer.h" file
#### Testing Procedure:
* The time length between two changes of LEDs can be customised by changing the defined constant 'DELAY' inside "timer.h" file (unit milliseconds).
* During the setted time length, even if the button is pressed there should not be any change in the LEDs.
     (i.e. All button press would be ignored during the fixed delay time)
* Current LED bit mask can be accessed by the function get_current_led(), this function takes an input of a uint8_t pointer to store the current bitmask.
    
### Task 2:
Author Luke Marinaki
#### Exercise: 
a-d) 
* Create a module interfacing with the STM32 board's USART1, which allows for receiving and transmitting serial data to an interface on a computer
* utilise interrupts to trigger the reading of the sent data. 
* Use a double buffer to ensure data is not overwritten while in use in the event of a new message sent at the same time.
#### Instruction:
* send serial input to the STM32 board using a serial terminal, such as the inbuilt terminal
* maximum message length is determined by BUFFER_SIZE, and termination character for the string is stored as TERMINATION_CHAR in "config.h"
* string (copied from the input buffer), and the number of received chars are sent to callback function, which retransmits the string to ensure correct receiving. 
#### Testing Procedure:
* Once the termination char is received, the callback retransmits the string back to the terminal to verify receiving correctly
* if not retransmitting correctly, create a breakpoint in SerialReceiveChar, which is called as an interrupt every time a character is received
* else, only change TERMINATION_CHAR and BUFFER_SIZE in config.h to modify functionality of the program. 
### Task 3:
#### Exercise:  Create a module that can interface to hardware timers. 

a) Basic functionality: implement a timer software module that can trigger a callback function a regular interval. 
b) Implement a function that can reset the timer with a new period, use get/set functions to allow outside modules to change the period.
c) Advanced functionality: implement a function for the module that can trigger a one-shot event. This function should take as an input the delay in milliseconds, and the callback function pointer.
    
#### Instruction:
All three parts are included in the singular program.
Part A of the task will not require any user input as when the timer is started a callback function will be triggered at 
a regular interval. This interval is created as a global variable INTERVAL_MS and has a default value of 200ms. Thus,        every interval that elapses the program will run the callback function which is testing and visualised as a led chase        sequence that cycles around the ring of 8 leds.
    
Part B is where we use get/set functions to reset the timer with a new period. We have a set function in the main source     file that allows us to change the value of new_period which is a static variable declared in the configuration module,       where we set the value of new_period to the variable timer_period and then reset TIM3 hardware for the new period.
    
Part C  We use TIM2 for the one shot event which is where after a 1000ms delay all the leds are inverted for a duration      3200ms where the periodic timer stil runs and thus, we should observe two cycles around the leds and return back to the      normal routine.
    
#### High Level Information: 
![Alt Text](/images/task3.png "Function Flowchart")    
#### Main.c:
The program is structured to initialize the hardware first, configure the necessary interrupts, and then continuously        run while waiting for events (like button presses or timer expiries). It handles both periodic and one-shot tasks using      timers, with actions defined by callback functions. 

#### Interrupts.c:
This file contains the Interrupt Service Routines (ISRs) for handling specific events related to Timer 2, Timer 3, and       external interrupts from a button press. Below is a summary of each ISR and some key points for further explanation:

TIM2_IRQHandler: This function handles interrupts from Timer 2, which are specifically configured to deal with one-shot      events like timeouts or delays that require precise timing.

TIM3_IRQHandler: Handles periodic interrupts from Timer 3, which occur upon every overflow of the timer—used for             regularly scheduled tasks such as updating status indicators or performing routine measurements.

EXTI0_IRQHandler: Manages external interrupts generated by pressing a button, configured to perform an immediate action      such as toggling LEDs or starting a timer-based operation.

#### Config.c:
The functions in config.c are called during the initial setup phase of the application to prepare the system for its         runtime operations. 
    
Clock and GPIO Configuration:
Enables clocks for GPIO ports and timers, ensuring that these components have the power and clock signals needed to          operate
    
Board Initialization:
Initializes board-specific configurations, particularly setting up GPIO pins for output, which are likely used to            control LEDs.
    
Timer Configuration:
Sets up timers for both one-shot and periodic interrupts, defining how and when these timers should trigger interrupts.
    
Button Interrupt Configuration:
Configures how the system responds to external button presses, setting up the necessary hardware registers to detect and     respond to these events
    
LED and Timer Utility Functions:
Contains functions to toggle LEDs, set timer parameters like prescalers and auto-reload values, and more sophisticated       functions to handle complex LED patterns and timing operations
    
#### Testing Procedure:
    
### Task 4:
#### Exercise:
    *Using the modules developed in the previous tasks to make a project that can give the board commands through USART1.
#### Instruction:
    *There are four types of acceptable commands:
     1. led - to change the pattern of the LEDs by giving a 8 bits bitmask, with 1 represent on and 0 represent off.
     2. serial - transmit a string from serial port, in this program it would be transmitted through USART1.
     3. oneshot - creat a one-shot timer with a user specified time length in milliseconds.
     4. timer - creat a continuous timer with a user specified period in milliseconds.

    *The format of each command should follow:
     - led <8 bits bitmask with 0 or 1>
     - serial <the string to be transmitted>
     - oneshot <time in milliseconds>
     - timer <period in milliseconds>
     (Replace <> with arguments that follow the description inside)
#### Testing Procedure:
    LED configuration constants:

    Serial configuration constants:
    "config.h"
    - BUFFER_SIZE determines the number of characters can be sent in one message
    - TERMINATION_CHAR determines the ASCII value of the character to signify end of message
    Timer configuration constants:


## Note
    *The hardware used is STM32F3DISCOVERY.
    *This project uses the definition file provided by Dr. Stewart Worrall. 
     Link: (https://github.com/stewart-worrall/MTRX2700-2024/tree/374a795e74c5f97bf11a56df9f673c4c0feec6d1/stm32f303-definitions)
