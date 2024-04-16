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
    *Press the user input button and the LEDs would be changed.
    *The time between two changes is set to be 1 second by default setting.
    *The first time pressing the button would light up one LED, and any presses after that will shift the LED to the nearby location.
    *DO NOT make changes to any values inside the functions and definitions other than 'DELAY' inside "timer.h" file
#### Testing Procedure:
    *The time length between two changes of LEDs can be customised by changing the defined constant 'DELAY' inside "timer.h" file (unit milliseconds).
    *During the setted time length, even if the button is pressed there should not be any change in the LEDs.
     (i.e. All button press would be ignored during the fixed delay time)
    *Current LED bit mask can be accessed by the fucntion get_current_led(), this function takes an input of a uint8_t pointer to store the current bitmask.
    
### Task 2:
#### Exercise:
    *
#### Instruction:
    
#### Testing Procedure:
    
### Task 3:
#### Exercise:
    
#### Instruction:
    
#### Testing Procedure:
    
### Task 4:
#### Exercise:
    *Using the modules developed in the previous tasks to make a project that can give the board commands from
     USART1.
#### Instruction:
    *There are four types of acceptable commands:
     1. led - to change the pattern of the LEDs by giving a 8 bits bitmask, with 1 represent on and 0 represent off.
     2. serial - transmit a string from serial port, in this program it would be transmitted through USART1.
     3. oneshot - creat a one-shot timer with a user specified time length in milliseconds.
     4. timer - creat a continuous timer with a user specified period in milliseconds.
    *The format of each command should follow:
     led <8 bits bitmask with 0 or 1>
     serial <the string to be transmitted>
     oneshot <time in milliseconds>
     timer <period in milliseconds>
     (Replace <> with arguments that follow the description inside)
#### Testing Procedure:
    
## Note
    *The hardware used is STM32F3DISCOVERY.
    *This project uses the definition file provided by Dr. Stewart Worrall. 
     Link: (https://github.com/stewart-worrall/MTRX2700-2024/tree/374a795e74c5f97bf11a56df9f673c4c0feec6d1/stm32f303-definitions)