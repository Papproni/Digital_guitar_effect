################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ILI9341/ILI9341_GFX.c \
../Core/Src/ILI9341/ILI9341_STM32_Driver.c 

OBJS += \
./Core/Src/ILI9341/ILI9341_GFX.o \
./Core/Src/ILI9341/ILI9341_STM32_Driver.o 

C_DEPS += \
./Core/Src/ILI9341/ILI9341_GFX.d \
./Core/Src/ILI9341/ILI9341_STM32_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/ILI9341/%.o Core/Src/ILI9341/%.su: ../Core/Src/ILI9341/%.c Core/Src/ILI9341/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/Components/ili9341 -I../../Drivers/BSP/Components/Common -I"C:/Projects/STM32H755/display_workspace/stm32_ili9341_test/CM7/Core/Src/ILI9341" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-ILI9341

clean-Core-2f-Src-2f-ILI9341:
	-$(RM) ./Core/Src/ILI9341/ILI9341_GFX.d ./Core/Src/ILI9341/ILI9341_GFX.o ./Core/Src/ILI9341/ILI9341_GFX.su ./Core/Src/ILI9341/ILI9341_STM32_Driver.d ./Core/Src/ILI9341/ILI9341_STM32_Driver.o ./Core/Src/ILI9341/ILI9341_STM32_Driver.su

.PHONY: clean-Core-2f-Src-2f-ILI9341

