/*
 * can.h
 *
 *  Created on: Dec 4, 2024
 *      Author: nadgi
 */

#ifndef CAN_H_
#define CAN_H_

#include "global.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "cbfifo.h"
#include "can.h"
#include <assert.h>

void can_init(void);

void abort_tx_0(void);
void abort_tx_1(void);
void abort_tx_2(void);
void can_transmit_0(uint8_t data, uint16_t id);
void can_transmit_16bit_0(uint16_t data, uint16_t id);
void can_transmit_float(float data, uint16_t id);
#ifdef DEBUG
void can_test_transmit(void);
#endif
//extern cbfifo_t fifotest;
#endif /* CAN_H_ */
