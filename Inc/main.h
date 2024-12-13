/*
 * main.h
 *
 *  Created on: Nov 4, 2024
 *      Author: nadgi
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "global.h"
#include "init_functions.h"
#include "iic.h"
#include "can.h"
#include <math.h>
#define INPUT_BUFFER_SIZE 128
#define ARGUMENT_BUFFER_SIZE 128

cbfifo_t fifotx, fiforx;
cbfifo_t fifosv, fifobv, fifocu, fifopo;
cbfifo_t fifotest; // Define the global variable
#endif /* MAIN_H_ */
