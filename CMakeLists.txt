##cmake_minimum_required(VERSION 4.1)
##project(Project3_code C)
##
##set(CMAKE_C_STANDARD 11)
##
##add_executable(Project3_code main.c
##        STD_TYPES.h
##        BIT_MATH.h
##        Adc/Adc.h
##        Adc/Adc_Private.h
##        Adc/Adc.c
##        Nvic/Nvic.h
##        Nvic/Nvic.c
##        GPIO/GPIO.c
##        GPIO/GPIO.h
##        GPIO/GPIO_PRIVATE.h
##        RCC/RCC.c
##        RCC/RCC.h
##        RCC/RCC_PRIVATE.h
##        Timer/Timer.c
##        Timer/Timer.h
##        Timer/Timer_Private.h
##        Lcd/Lcd.c
##        Lcd/Lcd.h
##        StateMachine/StateMachine.c
##        StateMachine/StateMachine.h)
##
##add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
##        COMMAND arm-none-eabi-objcopy -O ihex
##        ${PROJECT_NAME}.elf
##        ${PROJECT_NAME}.hex
##        COMMENT "Generating HEX file"
##)
#cmake_minimum_required(VERSION 3.16)
#
## Must point to toolchain BEFORE project() line
#set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-toolchain.cmake)
#
#project(Project3_code C ASM)
#
#set(CMAKE_C_STANDARD 11)
#
## ----------------------------------------------------------------
## STM32F401 CPU flags
## ----------------------------------------------------------------
#set(CPU_FLAGS
#        -mcpu=cortex-m4
#        -mthumb
#        -mfpu=fpv4-sp-d16
#        -mfloat-abi=hard
#)
#
## ----------------------------------------------------------------
## Compiler flags
## ----------------------------------------------------------------
#add_compile_options(
#        ${CPU_FLAGS}
#        -Wall
#        -Wextra
#        -O0                  # No optimization — easier to debug
#        -g                   # Debug symbols
#        -ffunction-sections  # Put each function in its own section
#        -fdata-sections      # Put each variable in its own section
#        -DSTM32F401xE        # Device define
#)
#
## ----------------------------------------------------------------
## Source files
## ----------------------------------------------------------------
#add_executable(${PROJECT_NAME}
#        STM32-base/startup/STM32F4xx/STM32F401xE.s     # <-- startup file (see Step 3)
#        main.c
#        STD_TYPES.h
#        BIT_MATH.h
#        Adc/Adc.h
#        Adc/Adc_Private.h
#        Adc/Adc.c
#        Nvic/Nvic.h
#        Nvic/Nvic.c
#        GPIO/Gpio.c
#        GPIO/Gpio.h
#        GPIO/Gpio_Private.h
#        RCC/Rcc.c
#        RCC/Rcc.h
#        RCC/Rcc_Private.h
#        Timer/Timer.c
#        Timer/Timer.h
#        Timer/Timer_Private.h
#        Lcd/Lcd.c
#        Lcd/Lcd.h
#        StateMachine/StateMachine.c
#        StateMachine/StateMachine.h
#)
#
## ----------------------------------------------------------------
## Linker flags
## ----------------------------------------------------------------
#target_link_options(${PROJECT_NAME} PRIVATE
#        STM32-base/startup/STM32F4xx
#        STM32-base/startup
#        ${CPU_FLAGS}
#        -T${CMAKE_SOURCE_DIR}/STM32-base/linker/STM32F4xx/STM32F401xE.ld   # <-- linker script (see Step 4)
#        -Wl,--gc-sections        # Remove unused sections
#        -Wl,-Map=${PROJECT_NAME}.map  # Generate map file
#        -nostdlib                # No standard C runtime (bare metal)
#        -lc                      # Link libc (for memset etc.)
#        -lm                      # Link libm (math)
#        -lnosys                  # Stub syscalls (no OS)
#        --specs=nosys.specs
#)
#
## Tell assembler where to find startup_common.s
#target_compile_options(${PROJECT_NAME} PRIVATE
#        $<$<COMPILE_LANGUAGE:ASM>:
#        -I${CMAKE_SOURCE_DIR}/STM32-base/startup
#        -I${CMAKE_SOURCE_DIR}/STM32-base/startup/STM32F4xx
#        >
#)
#
## Tell linker where to find common.ld
#target_link_options(${PROJECT_NAME} PRIVATE
#        -L${CMAKE_SOURCE_DIR}/STM32-base/linker
#        -L${CMAKE_SOURCE_DIR}/STM32-base/linker/STM32F4xx
#)
#
## ----------------------------------------------------------------
## Generate HEX file after build
## ----------------------------------------------------------------
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_OBJCOPY} -O ihex
#        $<TARGET_FILE:${PROJECT_NAME}>
#        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
#        COMMENT "Generating HEX file -> ${PROJECT_NAME}.hex"
#)
#
## ----------------------------------------------------------------
## Print size after build
## ----------------------------------------------------------------
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}>
#        COMMENT "Binary size:"
#)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-toolchain.cmake)
project(Project3_code C ASM)
set(CMAKE_C_STANDARD 11)

set(CPU_FLAGS -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)

add_compile_options(
        ${CPU_FLAGS}
        -Wall -Wextra -O0 -g
        -ffunction-sections
        -fdata-sections
        -DSTM32F401xE
)

add_executable(${PROJECT_NAME}
        startup.s
        main.c
        Adc/Adc.c
        Nvic/Nvic.c
        GPIO/Gpio.c
        RCC/Rcc.c
        Timer/Timer.c
        Lcd/Lcd.c
        StateMachine/StateMachine.c
)

target_link_options(${PROJECT_NAME} PRIVATE
        ${CPU_FLAGS}
        -T${CMAKE_SOURCE_DIR}/STM32F401.ld
        -Wl,--gc-sections
        -nostdlib
        --specs=nosys.specs
        -lnosys
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
        COMMENT "Generating HEX file"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}>
        COMMENT "Binary size:"
)