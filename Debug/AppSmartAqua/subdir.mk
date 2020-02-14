################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../AppSmartAqua/AppControll.c \
../AppSmartAqua/AppDisplay.c \
../AppSmartAqua/AppIO.c \
../AppSmartAqua/AppIhm.c \
../AppSmartAqua/AppLCD16x2.c \
../AppSmartAqua/AppPwm.c \
../AppSmartAqua/AppRtc.c \
../AppSmartAqua/AppShell.c \
../AppSmartAqua/AppTemperature.c \
../AppSmartAqua/AppTimer.c 

OBJS += \
./AppSmartAqua/AppControll.o \
./AppSmartAqua/AppDisplay.o \
./AppSmartAqua/AppIO.o \
./AppSmartAqua/AppIhm.o \
./AppSmartAqua/AppLCD16x2.o \
./AppSmartAqua/AppPwm.o \
./AppSmartAqua/AppRtc.o \
./AppSmartAqua/AppShell.o \
./AppSmartAqua/AppTemperature.o \
./AppSmartAqua/AppTimer.o 

C_DEPS += \
./AppSmartAqua/AppControll.d \
./AppSmartAqua/AppDisplay.d \
./AppSmartAqua/AppIO.d \
./AppSmartAqua/AppIhm.d \
./AppSmartAqua/AppLCD16x2.d \
./AppSmartAqua/AppPwm.d \
./AppSmartAqua/AppRtc.d \
./AppSmartAqua/AppShell.d \
./AppSmartAqua/AppTemperature.d \
./AppSmartAqua/AppTimer.d 


# Each subdirectory must supply rules for building sources it contributes
AppSmartAqua/%.o: ../AppSmartAqua/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DCPU_MKE06Z128VLD4_cm0plus -DFSL_RTOS_FREE_RTOS -DCPU_MKE06Z128VLD4 -DSDK_OS_FREE_RTOS -I../CMSIS -I../board -I../drivers -I../freertos/include -I../freertos/portable -I../source -I../utilities -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


