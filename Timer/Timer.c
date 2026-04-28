//
// Created by workstation on 4/27/2026.
//

#include "Timer.h"
#include "Timer_Private.h"
#include "../BIT_MATH.h"

/* ================================================================== */
/*  Internal: resolve Timer enum → peripheral pointer                 */
/* ================================================================== */
static TimerType *Timer_GetInstance(Timer_InstanceType Timer)
{
    TimerType *Instances[] =
    {
        (TimerType *)TIM2_BASE_ADDR,   /* TIMER_2 */
        (TimerType *)TIM3_BASE_ADDR,   /* TIMER_3 */
        (TimerType *)TIM4_BASE_ADDR,   /* TIMER_4 */
        (TimerType *)TIM5_BASE_ADDR    /* TIMER_5 */
    };
    return Instances[Timer];
}

/* ================================================================== */
/*  Internal: store ARR per timer instance so SetDutyCycle can use it */
/*  (we need ARR to compute CCR from a percentage)                    */
/* ================================================================== */
static uint16 Timer_ArrValues[4] = {0U, 0U, 0U, 0U};

/* ================================================================== */
/*  Internal: configure one channel's CCMR register for PWM Mode 1   */
/* ================================================================== */
static void Timer_ConfigChannel(TimerType *TIMx, Timer_ChannelType Channel)
{
    switch (Channel)
    {
        case TIMER_CHANNEL_1:
            /* Clear OC1M field (bits 6:4) then set PWM Mode 1 (110) */
            TIMx->CCMR1 &= ~(0x07UL << TIM_CCMR1_OC1M_POS);
            TIMx->CCMR1 |=  ((uint32)TIM_OCM_PWM1 << TIM_CCMR1_OC1M_POS);
            /* Enable output compare preload (OC1PE) */
            SET_BIT(TIMx->CCMR1, TIM_CCMR1_OC1PE);
            /* Enable channel output, active-high polarity */
            SET_BIT(TIMx->CCER, TIM_CCER_CC1E);
            CLR_BIT(TIMx->CCER, TIM_CCER_CC1P);
            break;

        case TIMER_CHANNEL_2:
            TIMx->CCMR1 &= ~(0x07UL << TIM_CCMR1_OC2M_POS);
            TIMx->CCMR1 |=  ((uint32)TIM_OCM_PWM1 << TIM_CCMR1_OC2M_POS);
            SET_BIT(TIMx->CCMR1, TIM_CCMR1_OC2PE);
            SET_BIT(TIMx->CCER, TIM_CCER_CC2E);
            CLR_BIT(TIMx->CCER, TIM_CCER_CC2P);
            break;

        case TIMER_CHANNEL_3:
            TIMx->CCMR2 &= ~(0x07UL << TIM_CCMR2_OC3M_POS);
            TIMx->CCMR2 |=  ((uint32)TIM_OCM_PWM1 << TIM_CCMR2_OC3M_POS);
            SET_BIT(TIMx->CCMR2, TIM_CCMR2_OC3PE);
            SET_BIT(TIMx->CCER, TIM_CCER_CC3E);
            CLR_BIT(TIMx->CCER, TIM_CCER_CC3P);
            break;

        case TIMER_CHANNEL_4:
            TIMx->CCMR2 &= ~(0x07UL << TIM_CCMR2_OC4M_POS);
            TIMx->CCMR2 |=  ((uint32)TIM_OCM_PWM1 << TIM_CCMR2_OC4M_POS);
            SET_BIT(TIMx->CCMR2, TIM_CCMR2_OC4PE);
            SET_BIT(TIMx->CCER, TIM_CCER_CC4E);
            CLR_BIT(TIMx->CCER, TIM_CCER_CC4P);
            break;

        default:
            break;
    }
}

/* ================================================================== */
/*                         API Implementations                        */
/* ================================================================== */

void Timer_PwmInit(Timer_InstanceType Timer, Timer_ChannelType Channel,
                   uint16 Psc, uint16 Arr)
{
    TimerType *TIMx = Timer_GetInstance(Timer);

    /* Stop counter while configuring */
    CLR_BIT(TIMx->CR1, TIM_CR1_CEN);

    /* Set prescaler and auto-reload */
    TIMx->PSC = (uint32)Psc;
    TIMx->ARR = (uint32)Arr;

    /* Store ARR so SetDutyCycle can use it */
    Timer_ArrValues[Timer] = Arr;

    /* Enable auto-reload preload so ARR changes take effect at next update */
    SET_BIT(TIMx->CR1, TIM_CR1_ARPE);

    /* Configure the requested channel for PWM Mode 1 */
    Timer_ConfigChannel(TIMx, Channel);

    /*
     * Force an update event to load PSC and ARR into their shadow registers
     * immediately. Without this, the first PWM period may be incorrect.
     */
    SET_BIT(TIMx->EGR, TIM_EGR_UG);

    /* Start the counter */
    SET_BIT(TIMx->CR1, TIM_CR1_CEN);
}

void Timer_SetDutyCycle(Timer_InstanceType Timer, Timer_ChannelType Channel,
                        uint8 DutyPercent)
{
    TimerType *TIMx = Timer_GetInstance(Timer);
    uint32 Ccr;

    /*
     * CCR = DutyPercent * (ARR + 1) / 100
     *
     * Example: ARR=999, DutyPercent=33
     * CCR = 33 * 1000 / 100 = 330
     * → Output is HIGH for 330/1000 = 33% of the period.
     *
     * We clamp to ARR+1 max to prevent CCR > ARR (would give ~100% but
     * the PWM output briefly glitches; cleaner to write ARR+1 = full on).
     */
    if (DutyPercent >= 100U)
    {
        Ccr = (uint32)Timer_ArrValues[Timer] + 1U;
    }
    else if (DutyPercent == 0U)
    {
        Ccr = 0U;
    }
    else
    {
        Ccr = ((uint32)DutyPercent * ((uint32)Timer_ArrValues[Timer] + 1U)) / 100U;
    }

    switch (Channel)
    {
        case TIMER_CHANNEL_1: TIMx->CCR1 = Ccr; break;
        case TIMER_CHANNEL_2: TIMx->CCR2 = Ccr; break;
        case TIMER_CHANNEL_3: TIMx->CCR3 = Ccr; break;
        case TIMER_CHANNEL_4: TIMx->CCR4 = Ccr; break;
        default: break;
    }
}

void Timer_Start(Timer_InstanceType Timer)
{
    SET_BIT(Timer_GetInstance(Timer)->CR1, TIM_CR1_CEN);
}

void Timer_Stop(Timer_InstanceType Timer)
{
    CLR_BIT(Timer_GetInstance(Timer)->CR1, TIM_CR1_CEN);
}