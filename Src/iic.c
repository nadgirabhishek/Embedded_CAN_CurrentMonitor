/*
 * iic.c
 *
 *  Created on: Dec 1, 2024
 *      Author: Abhishek Nadgir
 */

#include "iic.h"
#include "stm32f0xx.h"

#define GPIO_PIN_6 (1U << 6) // Bit 6
#define GPIO_PIN_7 (1U << 7) // Bit 7

#define SDA_PIN GPIO_PIN_7
#define SCL_PIN GPIO_PIN_6
#define I2C_PORT GPIOB

// Macros for controlling SDA and SCL
#define SDA_HIGH() (I2C_PORT->BSRR = SDA_PIN)
#define SDA_LOW() (I2C_PORT->BRR = SDA_PIN)
#define SCL_HIGH() (I2C_PORT->BSRR = SCL_PIN)
#define SCL_LOW() (I2C_PORT->BRR = SCL_PIN)
#define SDA_READ() ((I2C_PORT->IDR & SDA_PIN) != 0)

#define INA219_ADD 0x40
/*
 * I2C Initalization
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 */
void I2C_Init(void) {
    // Enable GPIOB clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Configure PB6 (SCL) and PB7 (SDA) as open-drain outputs
    I2C_PORT->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    I2C_PORT->MODER |= (GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0); // Output mode
    I2C_PORT->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);       // Open-drain
    I2C_PORT->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7); // High-speed

    // Enable internal pull-up resistors for PB6 (SCL) and PB7 (SDA)
    I2C_PORT->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7); // Clear bits
    I2C_PORT->PUPDR |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0); // Pull-up mode

    // Set both SDA and SCL high (idle state)
    SDA_HIGH();
    SCL_HIGH();
}
/*
 * I2c Start Condition
 *
 * Parameters:
 * None
 *
 * Returns:
 */
void I2C_Start(void) {
    SDA_HIGH();
    SCL_HIGH();
    for (volatile int i = 0; i < 10; i++); // Small delay
    SDA_LOW();
    for (volatile int i = 0; i < 10; i++);
    SCL_LOW();
}
/*
 * I2C Stop Condition
 *
 * Parameters:
 * None
 *
 * Returns:
 */
void I2C_Stop(void) {
    SCL_LOW();
    SDA_LOW();
    for (volatile int i = 0; i < 10; i++);
    SCL_HIGH();
    for (volatile int i = 0; i < 10; i++);
    SDA_HIGH();
}
/*
 * I2C Write condition
 *
 * Parameters:
 * 8 bit Data
 *
 * Returns:
 * Acknowledgment
 */
uint8_t I2C_Write(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        if (data & 0x80) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        data <<= 1;

        SCL_HIGH();
        for (volatile int j = 0; j < 10; j++); // Clock pulse delay
        SCL_LOW();
        for (volatile int j = 0; j < 10; j++);
    }

    // Read ACK/NACK
    SDA_HIGH(); // Release SDA
    SCL_HIGH();
    for (volatile int j = 0; j < 10; j++);
    uint8_t ack = !SDA_READ();
    SCL_LOW();
    return ack;
}
/*
 * I2c Read Function
 *
 * Parameters:
 * Acknowledgment
 *
 * Returns:
 * 8 bit Data
 */
uint8_t I2C_Read(uint8_t ack) {
    uint8_t data = 0;

    SDA_HIGH(); // Release SDA for input
    for (uint8_t i = 0; i < 8; i++) {
        data <<= 1;

        SCL_HIGH();
        for (volatile int j = 0; j < 10; j++); // Clock pulse delay
        if (SDA_READ()) {
            data |= 0x01;
        }
        SCL_LOW();
        for (volatile int j = 0; j < 10; j++);
    }

    // Send ACK or NACK
    if (ack) {
        SDA_LOW();
    } else {
        SDA_HIGH();
    }
    SCL_HIGH();
    for (volatile int j = 0; j < 10; j++);
    SCL_LOW();

    return data;
}
/*
 * Current Sensor Write Register
 *
 * Parameters:
 * 8 bit Register Addresss for current sensor and 16bit Data
 *
 * Returns:
 */
void INA219_WriteRegister(uint8_t reg, uint16_t value) {
    I2C_Start();
    I2C_Write(INA219_ADD << 1); // INA219 address with write bit
    I2C_Write(reg);       // Register address
    I2C_Write(value >> 8); // High byte
    I2C_Write(value & 0xFF); // Low byte
    I2C_Stop();
}
/*
 * Current Sensor Read Register
 *
 * Parameters:
 * 8 bit Register Addresss for current sensor
 *
 * Returns:
 * 16 bit DATA
 */
uint16_t INA219_ReadRegister(uint8_t reg) {
    uint16_t value;

    I2C_Start();
    I2C_Write(INA219_ADD << 1); // INA219 address with write bit
    I2C_Write(reg);       // Register address
    I2C_Start();
    I2C_Write((0x40 << 1) | 1); // INA219 address with read bit
    value = (I2C_Read(1) << 8); // Read high byte with ACK
    value |= I2C_Read(0);       // Read low byte with NACK
    I2C_Stop();

    return value;
}
