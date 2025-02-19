# STM32F091RC Nucleo Project for Current Sensor Interface and CAN Comm Protocol

## Overview
This project demonstrates a modular firmware design for an STM32F091RC Nucleo board. The project supports both transmitter and receiver roles, configurable via `#define` directives in the `main.c` file. It also provides functionality for debugging, UART-based data communication, and testing protocols. The firmware allows:

- Configurable roles as **Transmitter** or **Receiver**.
- Debug mode testing for the communication protocol.
- Adjustable sensor configurations for INA219 current sensor.
- Basic command processor and circular buffer FIFO (CBFIFO) for testing and data handling.

Below is the **block diagram** of the system:
![System Block Diagram](PES.jpg)

## Key Features

### Role Configuration
Set the device role in `main.c` using the following directive:

```c
#define TRANSMITTER // or #define RECEIVER
```

### Debug Mode
Enable or disable debug mode for testing:
- Debug mode is enabled by default in build configurations.
- Debug features allow CAN protocol testing and enhanced logging during development.

### Baud Rate Configuration
Set the UART baud rate using `#ifdef` directives in `init.functions.h`:

Default baud rates include 19200 with odd parity and 1 stop bit.

### Sensor Configuration (INA219 Current Sensor)
The INA219 sensor can be configured for different voltage ranges using `#ifdef` directives in `ina219.c`:

```c
#ifdef RANGE_16V
// Configure INA219 for 16V full scale
#endif

#ifdef RANGE_32V
// Configure INA219 for 32V full scale
#endif
```

### Basic Utilities
- **Command Processor**: Handles user commands for system testing and debugging. One can call these functions as per their need.
- **CBFIFO**: Implements circular buffer-based FIFO for efficient data handling.
- **UART Communication**: Prints data and debug information to a connected UART terminal.

### CAN IDs and Masks
- CAN message IDs and masks can be found in their respective header files (e.g., `can_ids.h`).
- If IDs are changed, ensure that corresponding values in the initialization function are also updated in `can.c` and `global.c` files.

## Hardware Connections
### STM32F091RC Nucleo Pin Configuration

#### I2C Interface (for INA219 Sensor)
| Function | Pin Number |
|----------|------------|
| I2C SDA  | PB7        |
| I2C SCL  | PB6        |

#### CAN Communication
| Function | Pin Number |
|----------|------------|
| CAN RX   | PB8      |
| CAN TX   | PB9       |

#### UART Communication
| Function | Pin Number |
|----------|------------|
| UART TX  | PA2        |
| UART RX  | PA3        |

### INA219 Sensor Connections
| Sensor Pin  | STM32 Pin |
|-------------|-----------|
| VCC         | 3.3V      |
| GND         | GND       |
| SDA         | PB7       |
| SCL         | PB6       |

## How to Use

1. Clone the repository and load the project onto your STM32F091RC Nucleo board.
2. Open the `main.c` file and set the role (`TRANSMITTER` or `RECEIVER`) using the `#define` directive.
3. If testing, ensure debug mode is enabled in your build configuration.
4. Adjust baud rate settings in `init.functions.h` by uncommenting the desired `#define`.
5. Configure the INA219 sensor for the desired voltage range in `ina219.c`.
6. Connect the hardware as per the connection details provided above.
7. Build and flash the firmware onto the STM32F091RC Nucleo board.
8. Monitor UART communication via a serial terminal (e.g., Tera Term or PuTTY).

## Notes
- Ensure proper termination and pull-up resistors for I2C lines if required.
- Ensure proper termination of CANH and CANL with 120 ohm resistors.
- Debug mode introduces additional testing and may affect real-time startup time.
- The provided command processor and CBFIFO are basic utilities from earlier assignments, designed for initial testing and can be extended for advanced use cases.
- CAN IDs and masks can be changed in the respective header files. If IDs are updated, corresponding values in the initialization functions must also be modified in `can.c` and `global.c`.
