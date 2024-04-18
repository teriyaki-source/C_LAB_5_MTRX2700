/*
 * flags.h
 *
 *  Created on: Apr 15, 2024
 *      Author: lukemarinaki
 */

#ifndef FLAGS_H_
#define FLAGS_H_

#include "stm32f303xc.h"
#include "config.h"
#include "serial.h"

#define ON 1
#define OFF 0

struct _Flags;
typedef struct _Flags Flags;

struct _Timer_Flags;
typedef struct _Timer_Flags Timer_flags;

//function get called when received a terminate character from serial port
//Process the message and set flags
void message_handler(uint8_t* instruction, uint8_t *action);
//raise the oneshot timer flag
void set_oneshot_flag();
//raise the continuous timer flag
void set_continuous_flag();
//function that check the flags status and do operations to respond it
void timer_handler();
//check all the flags and call functions to respond them and clear the flag if flag raised
void flag_handler();



#endif /* FLAGS_H_ */
