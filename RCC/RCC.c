//
// Created by alaa on 4/27/2026.
//

#include "Rcc.h"
#include "Rcc_Private.h"
#include "../BIT_MATH.h"

/* ================================================================== */
/*  Internal: maps a Rcc_PeripheralType to its enable register + bit  */
/* ================================================================== */
typedef enum
{
    BUS_AHB1 = 0U,
    BUS_APB1 = 1U,
    BUS_APB2 = 2U
} Rcc_BusType;

typedef struct
{
    Rcc_BusType Bus;
    uint8       BitPosition;
} Rcc_PeripheralMapEntry;

/*
 * Table must match the order of Rcc_PeripheralType enum exactly.
 * If you add a peripheral to the enum, add its entry here too.
 */
static const Rcc_PeripheralMapEntry Rcc_PeripheralMap[] =
{
    /* AHB1 */
    { BUS_AHB1, RCC_AHB1ENR_GPIOAEN  },  /* RCC_PERIPHERAL_GPIOA  */
    { BUS_AHB1, RCC_AHB1ENR_GPIOBEN  },  /* RCC_PERIPHERAL_GPIOB  */
    { BUS_AHB1, RCC_AHB1ENR_GPIOCEN  },  /* RCC_PERIPHERAL_GPIOC  */
    { BUS_AHB1, RCC_AHB1ENR_GPIODEN  },  /* RCC_PERIPHERAL_GPIOD  */
    { BUS_AHB1, RCC_AHB1ENR_GPIOEEN  },  /* RCC_PERIPHERAL_GPIOE  */
    { BUS_AHB1, RCC_AHB1ENR_GPIOHEN  },  /* RCC_PERIPHERAL_GPIOH  */
    { BUS_AHB1, RCC_AHB1ENR_DMA1EN   },  /* RCC_PERIPHERAL_DMA1   */
    { BUS_AHB1, RCC_AHB1ENR_DMA2EN   },  /* RCC_PERIPHERAL_DMA2   */

    /* APB1 */
    { BUS_APB1, RCC_APB1ENR_TIM2EN   },  /* RCC_PERIPHERAL_TIM2   */
    { BUS_APB1, RCC_APB1ENR_TIM3EN   },  /* RCC_PERIPHERAL_TIM3   */
    { BUS_APB1, RCC_APB1ENR_TIM4EN   },  /* RCC_PERIPHERAL_TIM4   */
    { BUS_APB1, RCC_APB1ENR_TIM5EN   },  /* RCC_PERIPHERAL_TIM5   */

    /* APB2 */
    { BUS_APB2, RCC_APB2ENR_ADC1EN   },  /* RCC_PERIPHERAL_ADC1   */
    { BUS_APB2, RCC_APB2ENR_TIM1EN   },  /* RCC_PERIPHERAL_TIM1   */
    { BUS_APB2, RCC_APB2ENR_SYSCFGEN  }, /* RCC_PERIPHERAL_SYSCFG */
};

/* ================================================================== */
/*  Helper: get a pointer to the correct ENR register                 */
/* ================================================================== */
static volatile uint32 *Rcc_GetEnrRegister(Rcc_BusType Bus)
{
    switch (Bus)
    {
        case BUS_AHB1: return &RCC->AHB1ENR;
        case BUS_APB1: return &RCC->APB1ENR;
        case BUS_APB2: return &RCC->APB2ENR;
        default:       return &RCC->AHB1ENR; /* unreachable, silence warning */
    }
}

/* ================================================================== */
/*                         API Implementations                        */
/* ================================================================== */

void Rcc_InitSysClock(void)
{
    /*
     * Use HSI (16 MHz internal RC oscillator) as SYSCLK.
     *
     * The HSI is already ON after reset (HSION=1, HSIRDY=1).
     * SYSCLK is already using HSI after reset (SW = 00).
     * So technically this function is a no-op for HSI,
     * but it's kept here explicitly so main.c has a clear
     * "clock init" step, and so you can swap in PLL config later.
     *
     * If you later add a crystal: enable HSE here, wait for HSERDY,
     * configure PLLCFGR, enable PLL, wait for PLLRDY, then switch SW.
     */

    /* Ensure HSI is ON (it should already be, but be explicit) */
    SET_BIT(RCC->CR, RCC_CR_HSION);

    /* Wait until HSI is stable */
    while (!READ_BIT(RCC->CR, RCC_CR_HSIRDY))
    {
        /* spin */
    }

    /* SW = 00: select HSI as SYSCLK (already the reset default) */
    CLR_BIT(RCC->CFGR, RCC_CFGR_SW0);
    CLR_BIT(RCC->CFGR, RCC_CFGR_SW1);
}

void Rcc_EnableClock(Rcc_PeripheralType Peripheral)
{
    volatile uint32 *ENR = Rcc_GetEnrRegister(Rcc_PeripheralMap[Peripheral].Bus);
    SET_BIT(*ENR, Rcc_PeripheralMap[Peripheral].BitPosition);

    /*
     * RM0368 note: after enabling a peripheral clock, there is a
     * 1-2 AHB cycle delay before the peripheral registers are accessible.
     * Reading the register back acts as the required delay barrier.
     */
    (void)*ENR;
}

void Rcc_DisableClock(Rcc_PeripheralType Peripheral)
{
    volatile uint32 *ENR = Rcc_GetEnrRegister(Rcc_PeripheralMap[Peripheral].Bus);
    CLR_BIT(*ENR, Rcc_PeripheralMap[Peripheral].BitPosition);
}