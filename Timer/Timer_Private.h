//
// Created by workstation on 4/27/2026.
//

#ifndef PROJECT3_CODE_TIMER_PRIVATE_H
#define PROJECT3_CODE_TIMER_PRIVATE_H
#include "../STD_TYPES.h"

/* ================================================================== */
/*  General-Purpose Timer Register Map  (TIM2-TIM5, RM0368 Chap 13)  */
/* ================================================================== */
typedef struct
{
    volatile uint32 CR1;    /* 0x00 - Control register 1               */
    volatile uint32 CR2;    /* 0x04 - Control register 2               */
    volatile uint32 SMCR;   /* 0x08 - Slave mode control register      */
    volatile uint32 DIER;   /* 0x0C - DMA/Interrupt enable register    */
    volatile uint32 SR;     /* 0x10 - Status register                  */
    volatile uint32 EGR;    /* 0x14 - Event generation register        */
    volatile uint32 CCMR1;  /* 0x18 - Capture/compare mode register 1  */
    volatile uint32 CCMR2;  /* 0x1C - Capture/compare mode register 2  */
    volatile uint32 CCER;   /* 0x20 - Capture/compare enable register  */
    volatile uint32 CNT;    /* 0x24 - Counter                          */
    volatile uint32 PSC;    /* 0x28 - Prescaler                        */
    volatile uint32 ARR;    /* 0x2C - Auto-reload register             */
    uint32          _RESERVED;
    volatile uint32 CCR1;   /* 0x34 - Capture/compare register 1       */
    volatile uint32 CCR2;   /* 0x38 - Capture/compare register 2       */
    volatile uint32 CCR3;   /* 0x3C - Capture/compare register 3       */
    volatile uint32 CCR4;   /* 0x40 - Capture/compare register 4       */
    uint32          _RESERVED2;
    volatile uint32 DCR;    /* 0x48 - DMA control register             */
    volatile uint32 DMAR;   /* 0x4C - DMA address for full transfer    */
    volatile uint32 TIM2OR; /* 0x50 - TIM2 option register (TIM2 only) */
} TimerType;

/* ------------------------------------------------------------------ */
/*  Base Addresses                                                     */
/* ------------------------------------------------------------------ */
#define TIM2_BASE_ADDR      0x40000000UL
#define TIM3_BASE_ADDR      0x40000400UL
#define TIM4_BASE_ADDR      0x40000800UL
#define TIM5_BASE_ADDR      0x40000C00UL

/* ------------------------------------------------------------------ */
/*  CR1 bit positions                                                  */
/* ------------------------------------------------------------------ */
#define TIM_CR1_CEN         0U     /* Counter enable                  */
#define TIM_CR1_UDIS        1U     /* Update disable                  */
#define TIM_CR1_URS         2U     /* Update request source           */
#define TIM_CR1_OPM         3U     /* One-pulse mode                  */
#define TIM_CR1_ARPE        7U     /* Auto-reload preload enable      */

/* ------------------------------------------------------------------ */
/*  CCMR1 bit positions  (Output Compare mode, channel 1 & 2)         */
/* ------------------------------------------------------------------ */
/* Channel 1 occupies bits [7:0], Channel 2 bits [15:8] */
#define TIM_CCMR1_OC1PE     3U     /* OC1 preload enable              */
#define TIM_CCMR1_OC1M_POS  4U    /* OC1M field position (3 bits)    */
#define TIM_CCMR1_OC2PE    11U     /* OC2 preload enable              */
#define TIM_CCMR1_OC2M_POS 12U    /* OC2M field position (3 bits)    */

/* ------------------------------------------------------------------ */
/*  CCMR2 bit positions  (Output Compare mode, channel 3 & 4)         */
/* ------------------------------------------------------------------ */
#define TIM_CCMR2_OC3PE     3U
#define TIM_CCMR2_OC3M_POS  4U
#define TIM_CCMR2_OC4PE    11U
#define TIM_CCMR2_OC4M_POS 12U

/* ------------------------------------------------------------------ */
/*  OCxM values — placed in the 3-bit OCxM field                      */
/* ------------------------------------------------------------------ */
#define TIM_OCM_FROZEN      0x00U  /* 000: Frozen (no change on match)*/
#define TIM_OCM_ACTIVE      0x01U  /* 001: Set on match               */
#define TIM_OCM_INACTIVE    0x02U  /* 010: Clear on match             */
#define TIM_OCM_TOGGLE      0x03U  /* 011: Toggle on match            */
#define TIM_OCM_PWM1        0x06U  /* 110: PWM mode 1 (active while CNT<CCR) */
#define TIM_OCM_PWM2        0x07U  /* 111: PWM mode 2 (inactive while CNT<CCR) */

/* ------------------------------------------------------------------ */
/*  CCER bit positions  (capture/compare output enable + polarity)     */
/* ------------------------------------------------------------------ */
#define TIM_CCER_CC1E       0U     /* CH1 output enable               */
#define TIM_CCER_CC1P       1U     /* CH1 polarity (0=active high)    */
#define TIM_CCER_CC2E       4U     /* CH2 output enable               */
#define TIM_CCER_CC2P       5U
#define TIM_CCER_CC3E       8U     /* CH3 output enable               */
#define TIM_CCER_CC3P       9U
#define TIM_CCER_CC4E      12U     /* CH4 output enable               */
#define TIM_CCER_CC4P      13U

/* ------------------------------------------------------------------ */
/*  EGR bit positions                                                  */
/* ------------------------------------------------------------------ */
#define TIM_EGR_UG          0U     /* Update generation (force reload)*/


#endif //PROJECT3_CODE_TIMER_PRIVATE_H