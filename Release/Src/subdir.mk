################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/can.c \
../Src/cbfifo.c \
../Src/commands.c \
../Src/functions.c \
../Src/global.c \
../Src/iic.c \
../Src/ina219.c \
../Src/init_functions.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/uart_functions.c 

OBJS += \
./Src/can.o \
./Src/cbfifo.o \
./Src/commands.o \
./Src/functions.o \
./Src/global.o \
./Src/iic.o \
./Src/ina219.o \
./Src/init_functions.o \
./Src/main.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/uart_functions.o 

C_DEPS += \
./Src/can.d \
./Src/cbfifo.d \
./Src/commands.d \
./Src/functions.d \
./Src/global.d \
./Src/iic.d \
./Src/ina219.d \
./Src/init_functions.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/uart_functions.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -DSTM32 -DSTM32F0 -DNUCLEO_F091RC -DSTM32F091RCTx -DSTM32F091xC -c -I../Inc -I../CMSIS -O0 -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/can.cyclo ./Src/can.d ./Src/can.o ./Src/can.su ./Src/cbfifo.cyclo ./Src/cbfifo.d ./Src/cbfifo.o ./Src/cbfifo.su ./Src/commands.cyclo ./Src/commands.d ./Src/commands.o ./Src/commands.su ./Src/functions.cyclo ./Src/functions.d ./Src/functions.o ./Src/functions.su ./Src/global.cyclo ./Src/global.d ./Src/global.o ./Src/global.su ./Src/iic.cyclo ./Src/iic.d ./Src/iic.o ./Src/iic.su ./Src/ina219.cyclo ./Src/ina219.d ./Src/ina219.o ./Src/ina219.su ./Src/init_functions.cyclo ./Src/init_functions.d ./Src/init_functions.o ./Src/init_functions.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/uart_functions.cyclo ./Src/uart_functions.d ./Src/uart_functions.o ./Src/uart_functions.su

.PHONY: clean-Src

