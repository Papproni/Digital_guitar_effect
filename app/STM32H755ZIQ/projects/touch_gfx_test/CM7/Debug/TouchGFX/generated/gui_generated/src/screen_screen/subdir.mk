################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/gui_generated/src/screen_screen/screenViewBase.cpp 

OBJS += \
./TouchGFX/generated/gui_generated/src/screen_screen/screenViewBase.o 

CPP_DEPS += \
./TouchGFX/generated/gui_generated/src/screen_screen/screenViewBase.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/gui_generated/src/screen_screen/%.o TouchGFX/generated/gui_generated/src/screen_screen/%.su: ../TouchGFX/generated/gui_generated/src/screen_screen/%.cpp TouchGFX/generated/gui_generated/src/screen_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-generated-2f-gui_generated-2f-src-2f-screen_screen

clean-TouchGFX-2f-generated-2f-gui_generated-2f-src-2f-screen_screen:
	-$(RM) ./TouchGFX/generated/gui_generated/src/screen_screen/screenViewBase.d ./TouchGFX/generated/gui_generated/src/screen_screen/screenViewBase.o ./TouchGFX/generated/gui_generated/src/screen_screen/screenViewBase.su

.PHONY: clean-TouchGFX-2f-generated-2f-gui_generated-2f-src-2f-screen_screen

