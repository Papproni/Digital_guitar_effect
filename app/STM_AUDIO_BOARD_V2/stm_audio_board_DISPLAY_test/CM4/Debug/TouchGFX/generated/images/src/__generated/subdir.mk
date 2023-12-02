################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_filler_small.cpp \
../TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_rounded_dark.cpp 

OBJS += \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_filler_small.o \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_rounded_dark.o 

CPP_DEPS += \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_filler_small.d \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_rounded_dark.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/images/src/__generated/%.o TouchGFX/generated/images/src/__generated/%.su: ../TouchGFX/generated/images/src/__generated/%.cpp TouchGFX/generated/images/src/__generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-generated-2f-images-2f-src-2f-__generated

clean-TouchGFX-2f-generated-2f-images-2f-src-2f-__generated:
	-$(RM) ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_filler_small.d ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_filler_small.o ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_filler_small.su ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_rounded_dark.d ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_rounded_dark.o ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_slider_horizontal_thick_rounded_dark.su

.PHONY: clean-TouchGFX-2f-generated-2f-images-2f-src-2f-__generated

