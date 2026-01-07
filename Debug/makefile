################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32U5xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Core/ST_Drivers/USB_DEVICE/Target/subdir.mk
-include Core/ST_Drivers/USB_DEVICE/App/subdir.mk
-include Core/ST_Drivers/Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
-include Core/ST_Drivers/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/subdir.mk
-include Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_Transmit/Src/subdir.mk
-include Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_Receive/Src/subdir.mk
-include Core/Bootloader_Drivers/USB_Ex_Driver/USB_Comm/USB_General/Src/subdir.mk
-include Core/Bootloader_Drivers/Boot_Driver/Src/subdir.mk
-include Core/Bootloader_Drivers/AT24C32_Driver/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := MiniPatchLambda_Bootloader
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
MiniPatchLambda_Bootloader.elf \

MAP_FILES += \
MiniPatchLambda_Bootloader.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
MiniPatchLambda_Bootloader.list \


# All Target
all: main-build

# Main-build Target
main-build: MiniPatchLambda_Bootloader.elf secondary-outputs

# Tool invocations
MiniPatchLambda_Bootloader.elf MiniPatchLambda_Bootloader.map: $(OBJS) $(USER_OBJS) C:\Users\Fatih\STM32CubeIDE\LW_MiniPatch_Lambda\MiniPatchLambda_Bootloader\STM32U5A5VJTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "MiniPatchLambda_Bootloader.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m33 -T"C:\Users\Fatih\STM32CubeIDE\LW_MiniPatch_Lambda\MiniPatchLambda_Bootloader\STM32U5A5VJTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="MiniPatchLambda_Bootloader.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

MiniPatchLambda_Bootloader.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "MiniPatchLambda_Bootloader.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) MiniPatchLambda_Bootloader.elf MiniPatchLambda_Bootloader.list MiniPatchLambda_Bootloader.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
