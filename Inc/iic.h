/*
 * iic.h
 *
 *  Created on: Dec 1, 2024
 *      Author: nadgi
 */

#ifndef IIC_H_
#define IIC_H_

#include "stm32f091xc.h"

void I2C_Init(void);
void I2C_Start(void);
uint8_t I2C_Write(uint8_t data);
uint8_t I2C_Read(uint8_t ack);
void I2C_Stop(void) ;
void INA219_WriteRegister(uint8_t reg, uint16_t value);
uint16_t INA219_ReadRegister(uint8_t reg);

#endif /* IIC_H_ */
