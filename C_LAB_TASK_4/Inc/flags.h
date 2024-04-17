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

void message_handler(uint8_t* instruction, uint8_t *action);

void set_oneshot_flag();

void set_continuous_flag();

void timer_handler();

void flag_handler();



#endif /* FLAGS_H_ */
