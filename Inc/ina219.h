/*
 * ina219.h
 *
 *  Created on: Dec 10, 2024
 *      Author: nadgi
 */

#ifndef INA219_H_
#define INA219_H_

#include "iic.h"
#include "global.h"

int INA219_Init(uint16_t config, uint16_t calibration);
// Read Bus Voltage
float INA219_ReadBusVoltage(void);
// Read Shunt Voltage
float INA219_ReadShuntVoltage(void);
// Read Power
float INA219_ReadPower(void);
// Read Current
float INA219_ReadCurrent(void);
// Read All Values
void INA219_ReadAll(float *shuntVoltage, float *busVoltage, float *current, float *power);

#endif /* INA219_H_ */
