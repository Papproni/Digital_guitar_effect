################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32h747igtx.s 

OBJS += \
./Core/Startup/startup_stm32h747igtx.o 

S_DEPS += \
./Core/Startup/startup_stm32h747igtx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -DDEBUG -c -I"C:/Users/Aron/Documents/GitHub/Digital_guitar_effect/app/STM_AUDIO_BOARD_V2/stm_audio_board_OCTAVE_test/Drivers/CMSIS/DSP/Include" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32h747igtx.d ./Core/Startup/startup_stm32h747igtx.o

.PHONY: clean-Core-2f-Startup

