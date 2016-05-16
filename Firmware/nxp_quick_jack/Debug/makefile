################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include example/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: nxp_quick_jack.axf

# Tool invocations
nxp_quick_jack.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/sam/LPCXpresso/ModulAid/nxp_quick_jack_board_lib/Debug" -L"/home/sam/LPCXpresso/ModulAid/lpc_chip_8xx_lib/Debug" -Xlinker -Map="nxp_quick_jack.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "nxp_quick_jack_Debug.ld" -o "nxp_quick_jack.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) nxp_quick_jack.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "nxp_quick_jack.axf"; # arm-none-eabi-objcopy -O binary "nxp_quick_jack.axf" "nxp_quick_jack.bin" ; checksum -p LPC812 -d "nxp_quick_jack.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
