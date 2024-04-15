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

struct _Flags;
typedef struct _Flags Flags;

void message_handler(uint8_t* instruction, uint8_t *action);

void flag_handler();

#endif /* FLAGS_H_ */
