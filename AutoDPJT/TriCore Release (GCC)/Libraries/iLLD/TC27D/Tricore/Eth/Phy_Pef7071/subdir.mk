################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC27D/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.c 

C_DEPS += \
./Libraries/iLLD/TC27D/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.d 

OBJS += \
./Libraries/iLLD/TC27D/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC27D/Tricore/Eth/Phy_Pef7071/%.o: ../Libraries/iLLD/TC27D/Tricore/Eth/Phy_Pef7071/%.c Libraries/iLLD/TC27D/Tricore/Eth/Phy_Pef7071/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AURIX GCC Compiler'
	tricore-elf-gcc -std=c99 "@C:/Users/USER/AURIX_PJT/AutoDPJT/TriCore Release (GCC)/AURIX_GCC_Compiler-Include_paths__-I_.opt" -O3 -Wall -c -fmessage-length=0 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections -mtc161 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Libraries-2f-iLLD-2f-TC27D-2f-Tricore-2f-Eth-2f-Phy_Pef7071

clean-Libraries-2f-iLLD-2f-TC27D-2f-Tricore-2f-Eth-2f-Phy_Pef7071:
	-$(RM) ./Libraries/iLLD/TC27D/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.d ./Libraries/iLLD/TC27D/Tricore/Eth/Phy_Pef7071/IfxEth_Phy_Pef7071.o

.PHONY: clean-Libraries-2f-iLLD-2f-TC27D-2f-Tricore-2f-Eth-2f-Phy_Pef7071

