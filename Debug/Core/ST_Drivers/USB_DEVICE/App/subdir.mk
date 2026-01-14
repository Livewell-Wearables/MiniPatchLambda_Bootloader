################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/ST_Drivers/USB_DEVICE/App/usb_device.c \
../Core/ST_Drivers/USB_DEVICE/App/usbd_cdc_if.c \
../Core/ST_Drivers/USB_DEVICE/App/usbd_desc.c 

OBJS += \
./Core/ST_Drivers/USB_DEVICE/App/usb_device.o \
./Core/ST_Drivers/USB_DEVICE/App/usbd_cdc_if.o \
./Core/ST_Drivers/USB_DEVICE/App/usbd_desc.o 

C_DEPS += \
./Core/ST_Drivers/USB_DEVICE/App/usb_device.d \
./Core/ST_Drivers/USB_DEVICE/App/usbd_cdc_if.d \
./Core/ST_Drivers/USB_DEVICE/App/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
Core/ST_Drivers/USB_DEVICE/App/%.o Core/ST_Drivers/USB_DEVICE/App/%.su Core/ST_Drivers/USB_DEVICE/App/%.cyclo: ../Core/ST_Drivers/USB_DEVICE/App/%.c Core/ST_Drivers/USB_DEVICE/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U5A5xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/ST_Drivers/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/ST_Drivers/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/ST_Drivers/USB_DEVICE/App" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/ST_Drivers/USB_DEVICE/Target" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/Boot_Driver/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/AT24C32_Driver/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_Receive/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_Transmit/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/CRC/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-ST_Drivers-2f-USB_DEVICE-2f-App

clean-Core-2f-ST_Drivers-2f-USB_DEVICE-2f-App:
	-$(RM) ./Core/ST_Drivers/USB_DEVICE/App/usb_device.cyclo ./Core/ST_Drivers/USB_DEVICE/App/usb_device.d ./Core/ST_Drivers/USB_DEVICE/App/usb_device.o ./Core/ST_Drivers/USB_DEVICE/App/usb_device.su ./Core/ST_Drivers/USB_DEVICE/App/usbd_cdc_if.cyclo ./Core/ST_Drivers/USB_DEVICE/App/usbd_cdc_if.d ./Core/ST_Drivers/USB_DEVICE/App/usbd_cdc_if.o ./Core/ST_Drivers/USB_DEVICE/App/usbd_cdc_if.su ./Core/ST_Drivers/USB_DEVICE/App/usbd_desc.cyclo ./Core/ST_Drivers/USB_DEVICE/App/usbd_desc.d ./Core/ST_Drivers/USB_DEVICE/App/usbd_desc.o ./Core/ST_Drivers/USB_DEVICE/App/usbd_desc.su

.PHONY: clean-Core-2f-ST_Drivers-2f-USB_DEVICE-2f-App

