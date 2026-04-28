//
// Created by workstation on 4/27/2026.
//

#ifndef PROJECT3_CODE_RCC_PRIVATE_H
#define PROJECT3_CODE_RCC_PRIVATE_H

#include "../STD_TYPES.h"

/* ================================================================== */
/*          RCC Register Map  (STM32F401, RM0368 Section 6.3)         */
/* ================================================================== */
typedef struct
{
    volatile uint32 CR;          /* 0x00 - Clock control register                  */
    volatile uint32 PLLCFGR;     /* 0x04 - PLL configuration register              */
    volatile uint32 CFGR;        /* 0x08 - Clock configuration register            */
    volatile uint32 CIR;         /* 0x0C - Clock interrupt register                */
    volatile uint32 AHB1RSTR;    /* 0x10 - AHB1 peripheral reset register          */
    volatile uint32 AHB2RSTR;    /* 0x14 - AHB2 peripheral reset register          */
    uint32          _RESERVED0[2];
    volatile uint32 APB1RSTR;    /* 0x20 - APB1 peripheral reset register          */
    volatile uint32 APB2RSTR;    /* 0x24 - APB2 peripheral reset register          */
    uint32          _RESERVED1[2];
    volatile uint32 AHB1ENR;     /* 0x30 - AHB1 peripheral clock enable register   */
    volatile uint32 AHB2ENR;     /* 0x34 - AHB2 peripheral clock enable register   */
    uint32          _RESERVED2[2];
    volatile uint32 APB1ENR;     /* 0x40 - APB1 peripheral clock enable register   */
    volatile uint32 APB2ENR;     /* 0x44 - APB2 peripheral clock enable register   */
    uint32          _RESERVED3[2];
    volatile uint32 AHB1LPENR;   /* 0x50 - AHB1 low-power clock enable             */
    volatile uint32 AHB2LPENR;   /* 0x54 - AHB2 low-power clock enable             */
    uint32          _RESERVED4[2];
    volatile uint32 APB1LPENR;   /* 0x60 - APB1 low-power clock enable             */
    volatile uint32 APB2LPENR;   /* 0x64 - APB2 low-power clock enable             */
    uint32          _RESERVED5[2];
    volatile uint32 BDCR;        /* 0x70 - Backup domain control register          */
    volatile uint32 CSR;         /* 0x74 - Clock control & status register         */
    uint32          _RESERVED6[2];
    volatile uint32 SSCGR;       /* 0x80 - Spread spectrum clock generation        */
    volatile uint32 PLLI2SCFGR;  /* 0x84 - PLLI2S configuration register           */
    uint32          _RESERVED7;
    volatile uint32 DCKCFGR;     /* 0x8C - Dedicated clocks configuration          */
} RccType;

#define RCC_BASE_ADDR       0x40023800UL
#define RCC                 ((RccType *)RCC_BASE_ADDR)

/* ------------------------------------------------------------------ */
/*  AHB1ENR bit positions  (RM0368 Section 6.3.12)                    */
/* ------------------------------------------------------------------ */
#define RCC_AHB1ENR_GPIOAEN     0U
#define RCC_AHB1ENR_GPIOBEN     1U
#define RCC_AHB1ENR_GPIOCEN     2U
#define RCC_AHB1ENR_GPIODEN     3U
#define RCC_AHB1ENR_GPIOEEN     4U
#define RCC_AHB1ENR_GPIOHEN     7U
#define RCC_AHB1ENR_DMA1EN     21U
#define RCC_AHB1ENR_DMA2EN     22U

/* ------------------------------------------------------------------ */
/*  APB1ENR bit positions  (RM0368 Section 6.3.16)                    */
/* ------------------------------------------------------------------ */
#define RCC_APB1ENR_TIM2EN      0U
#define RCC_APB1ENR_TIM3EN      1U
#define RCC_APB1ENR_TIM4EN      2U
#define RCC_APB1ENR_TIM5EN      3U
#define RCC_APB1ENR_USART2EN   17U
#define RCC_APB1ENR_I2C1EN     21U
#define RCC_APB1ENR_I2C2EN     22U
#define RCC_APB1ENR_I2C3EN     23U

/* ------------------------------------------------------------------ */
/*  APB2ENR bit positions  (RM0368 Section 6.3.17)                    */
/* ------------------------------------------------------------------ */
#define RCC_APB2ENR_TIM1EN      0U
#define RCC_APB2ENR_USART1EN    4U
#define RCC_APB2ENR_USART6EN    5U
#define RCC_APB2ENR_ADC1EN      8U
#define RCC_APB2ENR_SDIOEN     11U
#define RCC_APB2ENR_SPI1EN     12U
#define RCC_APB2ENR_SPI4EN     13U
#define RCC_APB2ENR_SYSCFGEN   14U
#define RCC_APB2ENR_TIM9EN     16U
#define RCC_APB2ENR_TIM10EN    17U
#define RCC_APB2ENR_TIM11EN    18U

/* ------------------------------------------------------------------ */
/*  CR bit positions                                                   */
/* ------------------------------------------------------------------ */
#define RCC_CR_HSION            0U
#define RCC_CR_HSIRDY           1U
#define RCC_CR_HSEON           16U
#define RCC_CR_HSERDY          17U
#define RCC_CR_PLLON           24U
#define RCC_CR_PLLRDY          25U

/* ------------------------------------------------------------------ */
/*  CFGR bit positions                                                 */
/* ------------------------------------------------------------------ */
#define RCC_CFGR_SW0            0U  /* System clock switch bit 0 */
#define RCC_CFGR_SW1            1U  /* System clock switch bit 1 */
#define RCC_CFGR_SWS0           2U  /* System clock switch status bit 0 */
#define RCC_CFGR_SWS1           3U  /* System clock switch status bit 1 */

#endif //PROJECT3_CODE_RCC_PRIVATE_H