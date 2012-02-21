################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../docs/IR/code.c 

OBJS += \
./docs/IR/code.o 

C_DEPS += \
./docs/IR/code.d 


# Each subdirectory must supply rules for building sources it contributes
docs/IR/%.o: ../docs/IR/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


