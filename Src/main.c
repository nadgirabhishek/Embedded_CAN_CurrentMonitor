/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include "main.h"

#define TRANSMITTER 1
#define RECEIVER 2
#define NODE_MODE TRANSMITTER

#define MAXSIZE 20 //buffer size for analysis

void Analyse_Responses(float *buffer, const char *type, int length);

int main(void) {
	SysTick_Init();
	I2C_Init();
	init_uled();
	cbfifo_init(&fiforx);
	cbfifo_init(&fifotx);
	cbfifo_init(&fifosv);
	cbfifo_init(&fifobv);
	cbfifo_init(&fifocu);
	cbfifo_init(&fifopo);
	can_init();
	uart_init();
#ifdef DEBUG
    cbfifo_init(&fifotest);
    can_test_transmit();
#endif
#if NODE_MODE == TRANSMITTER
	float shuntVoltageBuff[MAXSIZE], busVoltageBuff[MAXSIZE],
			currentBuff[MAXSIZE], powerBuff[MAXSIZE];
	float shuntVoltage, busVoltage, current, power;
	int i = 0, j = 0, k = 0, l = 0;
#endif
	printf("\nWelcome to Current Monitoring System!\n\r");

	while (1) {
#if NODE_MODE == TRANSMITTER
		if (read_all_flag) {
			read_all_flag = false;

			// Perform INA219 read and CAN transmit
			INA219_ReadAll(&shuntVoltage, &busVoltage, &current, &power);

			printf("Shunt Voltage: %.2f mV\n", shuntVoltage);
			printf("Bus Voltage: %.2f V\n", busVoltage);
			printf("Current: %.2f mA\n", current);
			printf("Power: %.2f mW\n", power);

			can_transmit_float(shuntVoltage, 0X100);
			can_transmit_float(busVoltage, 0X101);
			can_transmit_float(current, 0X102);
			can_transmit_float(power, 0X103);

			// Store values for analysis
			shuntVoltageBuff[i++] = shuntVoltage;
			busVoltageBuff[j++] = busVoltage;
			currentBuff[k++] = current;
			powerBuff[l++] = power;

			if (i == MAXSIZE && j == MAXSIZE && k == MAXSIZE && l == MAXSIZE) {
				Analyse_Responses(shuntVoltageBuff, "Shunt Voltage (mV)",
				MAXSIZE);
				Analyse_Responses(busVoltageBuff, "Bus Voltage (V)", MAXSIZE);
				Analyse_Responses(currentBuff, "Current (mA)", MAXSIZE);
				Analyse_Responses(powerBuff, "Power (mW)", MAXSIZE);

				// Reset indices
				i = j = k = l = 0;
			}
		}
#elif NODE_MODE == RECEIVER
		// Check if there’s anything to dequeue and process
		float dequeuedValue;

		if (cbfifo_dequeue_float(&fifosv, &dequeuedValue)) {
			printf("Dequeued Shunt Voltage: %.2f mV\n", dequeuedValue);
		}
		if (cbfifo_dequeue_float(&fifobv, &dequeuedValue)) {
			printf("Dequeued Bus Voltage: %.2f V\n", dequeuedValue);
		}
		if (cbfifo_dequeue_float(&fifocu, &dequeuedValue)) {
			printf("Dequeued Current: %.2f mA\n", dequeuedValue);
		}
		if (cbfifo_dequeue_float(&fifopo, &dequeuedValue)) {
			printf("Dequeued Power: %.2f mW\n", dequeuedValue);
		}
#endif// If NODE_MODE is NONE, do nothing
	}
}

void Analyse_Responses(float *buffer, const char *type, int length) {
	float min = buffer[0], max = buffer[0], sum = 0;

	for (int i = 0; i < length; i++) {
		if (buffer[i] < min)
			min = buffer[i];
		if (buffer[i] > max)
			max = buffer[i];
		sum += buffer[i];
	}

	float avg = sum / length;

	printf("Analysis for %s over last %d seconds:\n", type, length);
	printf("Min: %.2f, Max: %.2f, Avg: %.2f\n\r", min, max, avg);
}

