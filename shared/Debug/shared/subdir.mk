################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shared/net.c \
../shared/protocol.c \
../shared/utils.c 

OBJS += \
./shared/net.o \
./shared/protocol.o \
./shared/utils.o 

C_DEPS += \
./shared/net.d \
./shared/protocol.d \
./shared/utils.d 


# Each subdirectory must supply rules for building sources it contributes
shared/%.o: ../shared/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


