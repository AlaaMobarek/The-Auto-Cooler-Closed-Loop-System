//
// Created by workstation on 4/27/2026.
//

#ifndef PROJECT3_CODE_GPIO_PRIVATE_H
#define PROJECT3_CODE_GPIO_PRIVATE_H

#include "../STD_TYPES.h"

/* ================================================================== */
/*              GPIO Register Map  (STM32F401 Reference RM0368)       */
/* ================================================================== */
typedef struct
{
    volatile uint32 MODER;      /* 0x00 - Mode register                        */
    volatile uint32 OTYPER;     /* 0x04 - Output type register                 */
    volatile uint32 OSPEEDR;    /* 0x08 - Output speed register                */
    volatile uint32 PUPDR;      /* 0x0C - Pull-up/pull-down register           */
    volatile uint32 IDR;        /* 0x10 - Input data register                  */
    volatile uint32 ODR;        /* 0x14 - Output data register                 */
    volatile uint32 BSRR;       /* 0x18 - Bit set/reset register               */
    volatile uint32 LCKR;       /* 0x1C - Configuration lock register          */
    volatile uint32 AFRL;       /* 0x20 - Alternate function low  (pins 0-7)   */
    volatile uint32 AFRH;       /* 0x24 - Alternate function high (pins 8-15)  */
} GpioType;

/* ------------------------------------------------------------------ */
/*  Base addresses  (STM32F401 memory map, RM0368 Table 1)            */
/* ------------------------------------------------------------------ */
#define GPIOA_BASE_ADDR     0x40020000UL
#define GPIOB_BASE_ADDR     0x40020400UL
#define GPIOC_BASE_ADDR     0x40020800UL
#define GPIOD_BASE_ADDR     0x40020C00UL
#define GPIOE_BASE_ADDR     0x40021000UL
#define GPIOH_BASE_ADDR     0x40021C00UL

/* ------------------------------------------------------------------ */
/*  MODER  — 2 bits per pin  (bits [2n+1 : 2n])                       */
/* ------------------------------------------------------------------ */
#define GPIO_MODER_INPUT        0x00U   /* 00: Input (reset state)             */
#define GPIO_MODER_OUTPUT       0x01U   /* 01: General-purpose output          */
#define GPIO_MODER_AF           0x02U   /* 10: Alternate function              */
#define GPIO_MODER_ANALOG       0x03U   /* 11: Analog                          */

/* ------------------------------------------------------------------ */
/*  OTYPER — 1 bit per pin  (bit n)                                   */
/* ------------------------------------------------------------------ */
#define GPIO_OTYPE_PP           0x00U   /* 0: Push-pull  (reset state)         */
#define GPIO_OTYPE_OD           0x01U   /* 1: Open-drain                       */

/* ------------------------------------------------------------------ */
/*  OSPEEDR — 2 bits per pin  (bits [2n+1 : 2n])                      */
/* ------------------------------------------------------------------ */
#define GPIO_SPEED_LOW          0x00U   /* 00: Low speed (~2 MHz)              */
#define GPIO_SPEED_MEDIUM       0x01U   /* 01: Medium speed (~25 MHz)          */
#define GPIO_SPEED_HIGH         0x02U   /* 10: High speed (~50 MHz)            */
#define GPIO_SPEED_VERY_HIGH    0x03U   /* 11: Very high speed (~100 MHz)      */

/* ------------------------------------------------------------------ */
/*  PUPDR — 2 bits per pin  (bits [2n+1 : 2n])                        */
/* ------------------------------------------------------------------ */
#define GPIO_PUPD_NONE          0x00U   /* 00: No pull-up / pull-down          */
#define GPIO_PUPD_PULLUP        0x01U   /* 01: Pull-up                         */
#define GPIO_PUPD_PULLDOWN      0x02U   /* 10: Pull-down                       */
/* 11 is reserved                                                      */

/* ------------------------------------------------------------------ */
/*  AFR — 4 bits per pin                                              */
/*  AFRL covers pins 0-7, AFRH covers pins 8-15                       */
/* ------------------------------------------------------------------ */
#define GPIO_AF0                0x00U
#define GPIO_AF1                0x01U
#define GPIO_AF2                0x02U
#define GPIO_AF3                0x03U
#define GPIO_AF4                0x04U
#define GPIO_AF5                0x05U
#define GPIO_AF6                0x06U
#define GPIO_AF7                0x07U
#define GPIO_AF8                0x08U
#define GPIO_AF9                0x09U
#define GPIO_AF10               0x0AU
#define GPIO_AF11               0x0BU
#define GPIO_AF12               0x0CU
#define GPIO_AF13               0x0DU
#define GPIO_AF14               0x0EU
#define GPIO_AF15               0x0FU

/* ------------------------------------------------------------------ */
/*  BSRR layout                                                        */
/*  Bits [15:0]  = BSy  — write 1 to SET   pin y                      */
/*  Bits [31:16] = BRy  — write 1 to RESET pin y                      */
/* ------------------------------------------------------------------ */
#define GPIO_BSRR_SET(pin)      (1UL << (pin))
#define GPIO_BSRR_RESET(pin)    (1UL << ((pin) + 16U))


#endif //PROJECT3_CODE_GPIO_PRIVATE_H