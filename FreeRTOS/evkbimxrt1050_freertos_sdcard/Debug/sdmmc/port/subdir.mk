################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sdmmc/port/fsl_sdmmc_event.c \
../sdmmc/port/fsl_sdmmc_host.c 

OBJS += \
./sdmmc/port/fsl_sdmmc_event.o \
./sdmmc/port/fsl_sdmmc_host.o 

C_DEPS += \
./sdmmc/port/fsl_sdmmc_event.d \
./sdmmc/port/fsl_sdmmc_host.d 


# Each subdirectory must supply rules for building sources it contributes
sdmmc/port/%.o: ../sdmmc/port/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MIMXRT1052DVL6B -DCPU_MIMXRT1052DVL6B_cm7 -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 -DFSL_RTOS_FREE_RTOS -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../ -I../drivers -I../device -I../CMSIS -I../amazon-freertos/freertos/portable -I../amazon-freertos/include -I../sdmmc/port -I../sdmmc/inc -I../utilities -I../component/lists -I../component/serial_manager -I../component/uart -I../xip -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


