################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/gui/src/screen2_screen/Screen2Presenter.cpp \
../TouchGFX/gui/src/screen2_screen/Screen2View.cpp 

OBJS += \
./TouchGFX/gui/src/screen2_screen/Screen2Presenter.o \
./TouchGFX/gui/src/screen2_screen/Screen2View.o 

CPP_DEPS += \
./TouchGFX/gui/src/screen2_screen/Screen2Presenter.d \
./TouchGFX/gui/src/screen2_screen/Screen2View.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/gui/src/screen2_screen/%.o TouchGFX/gui/src/screen2_screen/%.su: ../TouchGFX/gui/src/screen2_screen/%.cpp TouchGFX/gui/src/screen2_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-gui-2f-src-2f-screen2_screen

clean-TouchGFX-2f-gui-2f-src-2f-screen2_screen:
	-$(RM) ./TouchGFX/gui/src/screen2_screen/Screen2Presenter.d ./TouchGFX/gui/src/screen2_screen/Screen2Presenter.o ./TouchGFX/gui/src/screen2_screen/Screen2Presenter.su ./TouchGFX/gui/src/screen2_screen/Screen2View.d ./TouchGFX/gui/src/screen2_screen/Screen2View.o ./TouchGFX/gui/src/screen2_screen/Screen2View.su

.PHONY: clean-TouchGFX-2f-gui-2f-src-2f-screen2_screen

