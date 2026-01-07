################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/USB_General.c 

OBJS += \
./Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/USB_General.o 

C_DEPS += \
./Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/USB_General.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/%.o Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/%.su Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/%.cyclo: ../Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/%.c Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U5A5xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/ST_Drivers/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/ST_Drivers/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/ST_Drivers/USB_DEVICE/App" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/ST_Drivers/USB_DEVICE/Target" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/Boot_Driver/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/AT24C32_Driver/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_Receive/Inc" -I"C:/Users/Fatih/STM32CubeIDE/LW_MiniPatch_Lambda/MiniPatchLambda_Bootloader/Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_Transmit/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Bootloader_Drivers-2f-USB_Ex_Driver-2f-USB_Comm-2f-USB_General-2f-Src

clean-Core-2f-Bootloader_Drivers-2f-USB_Ex_Driver-2f-USB_Comm-2f-USB_General-2f-Src:
	-$(RM) ./Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/USB_General.cyclo ./Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/USB_General.d ./Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/USB_General.o ./Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/USB_General.su

.PHONY: clean-Core-2f-Bootloader_Drivers-2f-USB_Ex_Driver-2f-USB_Comm-2f-USB_General-2f-Src

