/*
 * ina219.c
 *
 *  Created on: Dec 10, 2024
 *      Author: nadgi
 */

#include "ina219.h"

// INA219 Register Addresses
#define INA219_REG_CONFIG 0x00
#define INA219_REG_SHUNT_VOLTAGE 0x01
#define INA219_REG_BUS_VOLTAGE 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CALIBRATION 0x05

// Bus Voltage and Configuration Modes
//#define MODE_32V_BUS   // Uncomment for 32V bus configuration
#define MODE_16V_BUS // Uncomment for 16V bus configuration (comment MODE_32V_BUS when enabled)

// Common Configuration
#define SHUNT_VOLTAGE_LSB 0.01 // 10 µV per bit

#ifdef MODE_32V_BUS
    #define INA219_CONFIG 0x399F       // 32V bus, ±320mV shunt, 12-bit ADC, continuous mode
    #define BUS_VOLTAGE_LSB 0.004     // 4 mV per bit (minimum least count for 32V)
    #define POWER_LSB 0.02            // 20 mW per bit (default calibration assumption)
    #define CURRENT_LSB 1.0           // 1 mA per bit (default calibration assumption)
    #define INA219_CALIBRATION 0x1000 // Calibration for 1mA/bit with 0.1Ω shunt resistor
#elif defined(MODE_16V_BUS)
    #define INA219_CONFIG 0x199F       // 16V bus, ±320mV shunt, 12-bit ADC, continuous mode
    #define BUS_VOLTAGE_LSB 0.004      // 4 mV per bit (minimum least count for 16V)
    #define POWER_LSB 0.01             // 10 mW per bit (scaled for lower voltage)
    #define CURRENT_LSB 0.5            // 0.5 mA per bit (scaled for higher resolution)
    #define INA219_CALIBRATION 0x2000  // Calibration for 0.5mA/bit with 0.1Ω shunt resistor
#else
    #error "Please define either MODE_32V_BUS or MODE_16V_BUS"
#endif

/*INA219_Init
 * Current Sensor Initalization Function
 *
 * Parameters:
 * 	 Value to write in Configuration Register and Calibration Register.
 *
 * Returns:
 *   int 0 for success -1 and -2 for failure to initalize
 */
int INA219_Init(uint16_t config, uint16_t calibration) {
    // Write configuration register
    INA219_WriteRegister(INA219_REG_CONFIG, config);
    uint16_t readBackConfig = INA219_ReadRegister(INA219_REG_CONFIG);

    // Verify configuration register
    if (readBackConfig != config) {
        printf("Error: Config register mismatch. Written: 0x%04X, Read: 0x%04X\n", config, readBackConfig);
        return -1; // Error code for config mismatch
    }

    // Write calibration register
    INA219_WriteRegister(INA219_REG_CALIBRATION, calibration);
    uint16_t readBackCalibration = INA219_ReadRegister(INA219_REG_CALIBRATION);

    // Verify calibration register
    if (readBackCalibration != calibration) {
        printf("Error: Calibration register mismatch. Written: 0x%04X, Read: 0x%04X\n", calibration, readBackCalibration);
        return -2; // Error code for calibration mismatch
    }

    printf("INA219 Initialized successfully.\n");
    return 0; // Success
}
/*INA219_ReadBusVoltage
 *
 *
 * Parameters:
 * 	 None.
 *
 * Returns:
 *   Return Bus Voltage in float format
 */
float INA219_ReadBusVoltage(void) {
    uint16_t rawValue = INA219_ReadRegister(INA219_REG_BUS_VOLTAGE);
    // Check CNVR (Bit 1) and OVF (Bit 0)
    uint8_t cnvr = (rawValue & 0x02) >> 1; // Conversion Ready bit
    uint8_t ovf = rawValue & 0x01;         // Overflow bit
    if (!cnvr) {
        // Data is not ready; handle this as an error or retry
        printf("Error: Conversion not ready.\n");
        return -1.0; // Indicate an error with a negative value
    }
    if (ovf) {
        // Overflow occurred; handle this as an error
        printf("Error: Math overflow detected.\n");
        return -2.0; // Indicate an error with a distinct negative value
    }
    // Extract Bus Voltage Data (Bits 15-3)
    rawValue >>= 3; // Remove bits 2-0
    return rawValue * BUS_VOLTAGE_LSB; // Convert to volts
}
/*INA219_ReadShuntVoltage
 *
 *
 * Parameters:
 * 	 None.
 *
 * Returns:
 *   Shunt voltage as a Float
 */
float INA219_ReadShuntVoltage(void) {
    int16_t rawValue = INA219_ReadRegister(INA219_REG_SHUNT_VOLTAGE);
    return rawValue * SHUNT_VOLTAGE_LSB; // Convert to volts
}

/*INA219_ReadPower
 *
 *
 * Parameters:
 * 	 None.
 *
 * Returns:
 *   Returns Power as a float
 */
float INA219_ReadPower(void) {
    uint16_t rawValue = INA219_ReadRegister(INA219_REG_POWER);
    return rawValue * POWER_LSB; // Convert to watts
}

/*INA219_ReadCurrent
 *
 *
 * Parameters:
 * 	 None.
 *
 * Returns:
 *   Returns Current as float
 */
float INA219_ReadCurrent(void) {
    int16_t rawValue = INA219_ReadRegister(INA219_REG_CURRENT);
    return rawValue * CURRENT_LSB; // Convert to amperes
}
/*INA219_ReadAll
 *
 *
 * Parameters:
 * 	 Pointer to float for storing the values read from sensor
 *
 * Returns:
 *   None.
 */
void INA219_ReadAll(float *shuntVoltage, float *busVoltage, float *current, float *power) {
    static int initialized = 0;

    // Initialize I2C and INA219 if not already done
    if (!initialized) {
        I2C_Init(); // Initialize I2C communication
        if (INA219_Init(INA219_CONFIG, INA219_CALIBRATION) != 0) {
            printf("Error: Failed to initialize INA219.\n");
            *shuntVoltage = -1.0;
            *busVoltage = -1.0;
            *current = -1.0;
            *power = -1.0;
            return; // Abort if initialization fails
        }
        initialized = 1; // Mark as initialized
    }

    // Read values
    *shuntVoltage = INA219_ReadShuntVoltage();
    *busVoltage = INA219_ReadBusVoltage();
    *current = INA219_ReadCurrent();
    *power = INA219_ReadPower();
}
