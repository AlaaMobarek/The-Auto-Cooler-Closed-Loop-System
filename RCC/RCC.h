//
// Created by alaa on 4/27/2026.
//

#ifndef PROJECT3_CODE_RCC_H
#define PROJECT3_CODE_RCC_H

#include "../STD_TYPES.h"

/* ================================================================== */
/*                   Peripheral Clock Identifiers                     */
/*  These are passed to Rcc_EnableClock() / Rcc_DisableClock()        */
/* ================================================================== */
typedef enum
{
    /* --- AHB1 peripherals --- */
    RCC_PERIPHERAL_GPIOA = 0U,
    RCC_PERIPHERAL_GPIOB,
    RCC_PERIPHERAL_GPIOC,
    RCC_PERIPHERAL_GPIOD,
    RCC_PERIPHERAL_GPIOE,
    RCC_PERIPHERAL_GPIOH,
    RCC_PERIPHERAL_DMA1,
    RCC_PERIPHERAL_DMA2,

    /* --- APB1 peripherals --- */
    RCC_PERIPHERAL_TIM2,
    RCC_PERIPHERAL_TIM3,       /* Used for PWM fan control */
    RCC_PERIPHERAL_TIM4,
    RCC_PERIPHERAL_TIM5,

    /* --- APB2 peripherals --- */
    RCC_PERIPHERAL_ADC1,       /* Used for temperature sensor */
    RCC_PERIPHERAL_TIM1,
    RCC_PERIPHERAL_SYSCFG

} Rcc_PeripheralType;

/* ================================================================== */
/*                              API                                   */
/* ================================================================== */

/**
 * @brief  Initialize the system clock.
 *         On STM32F401 with no external crystal in simulation (Proteus),
 *         we use the internal HSI oscillator (16 MHz) directly as SYSCLK.
 *
 *  ⚠️  IMPORTANT NOTE FOR PROTEUS:
 *      Proteus does not simulate the PLL. If you configure the PLL, the
 *      PLLRDY flag will never set and your code will hang in a while() loop.
 *      This function deliberately uses HSI (16 MHz) only, which works in
 *      both Proteus and real hardware.
 *
 *      On real hardware with a crystal, replace this with your PLL setup.
 */
void Rcc_InitSysClock(void);

/**
 * @brief  Enable the peripheral clock for a given peripheral.
 * @param  Peripheral  One of the Rcc_PeripheralType enum values.
 */
void Rcc_EnableClock(Rcc_PeripheralType Peripheral);

/**
 * @brief  Disable the peripheral clock for a given peripheral.
 * @param  Peripheral  One of the Rcc_PeripheralType enum values.
 */
void Rcc_DisableClock(Rcc_PeripheralType Peripheral);

#endif //PROJECT3_CODE_RCC_H