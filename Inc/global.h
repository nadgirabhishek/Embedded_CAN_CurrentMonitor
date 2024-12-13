/*
 * global.h
 *
 *  Created on: Nov 4, 2024
 *      Author: Abhishek Nadgir
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stm32f0xx.h>
#include <stm32f091xc.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"
#include "utilities.h"
#include "cbfifo.h"
#include <stdint.h>
#include "cbfifo.h"

extern volatile uint32_t tick_counter ;
extern volatile bool read_all_flag;
extern cbfifo_t fifotx, fiforx;

extern cbfifo_t fiforx, fiforx;

extern cbfifo_t fifosv, fifosv;

extern cbfifo_t fifobv, fifobv;

extern cbfifo_t fifocu, fifocu;

extern cbfifo_t fifopo, fifopo;

extern cbfifo_t fifotest, fifotest;

void USART2_IRQHandler(void);
void CEC_CAN_IRQHandler(void);

#endif /* GLOBAL_H_ */
