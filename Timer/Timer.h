//
// Created by workstation on 4/27/2026.
//

#ifndef PROJECT3_CODE_TIMER_H
#define PROJECT3_CODE_TIMER_H

#include "../STD_TYPES.h"

/* ================================================================== */
/*                     Timer Identifiers                              */
/* ================================================================== */
typedef enum
{
    TIMER_2 = 0U,
    TIMER_3 = 1U,   /* <-- Used for fan PWM in this project           */
    TIMER_4 = 2U,
    TIMER_5 = 3U
} Timer_InstanceType;

/* ================================================================== */
/*                     PWM Channel Identifiers                        */
/* ================================================================== */
typedef enum
{
    TIMER_CHANNEL_1 = 1U,
    TIMER_CHANNEL_2 = 2U,   /* <-- TIM3_CH2 = PA7 (AF2) used for fan  */
    TIMER_CHANNEL_3 = 3U,
    TIMER_CHANNEL_4 = 4U
} Timer_ChannelType;

/* ================================================================== */
/*                         API                                        */
/* ================================================================== */

/**
 * @brief  Configure a timer channel for PWM output (PWM Mode 1).
 *
 *         PWM Mode 1: output is HIGH while CNT < CCR, LOW otherwise.
 *         Frequency = TimerClock / ((PSC+1) * (ARR+1))
 *
 *         For this project (HSI = 16 MHz, fan PWM at ~1 kHz):
 *           PSC = 15  → Timer clock = 16 MHz / 16 = 1 MHz
 *           ARR = 999 → PWM frequency = 1 MHz / 1000 = 1 kHz
 *
 * @param  Timer    Timer instance (TIMER_2 .. TIMER_5)
 * @param  Channel  PWM output channel (TIMER_CHANNEL_1 .. 4)
 * @param  Psc      Prescaler value  (timer counts at Fclk / (Psc+1))
 * @param  Arr      Auto-reload value (period = (Arr+1) timer ticks)
 *
 *  ⚠️  The GPIO pin must be configured for Alternate Function BEFORE
 *      calling this. TIM3_CH2 → PA7, AF2. Use Gpio_ConfigPin() first.
 *      RCC clock for the timer must also be enabled before this call.
 */
void Timer_PwmInit(Timer_InstanceType Timer, Timer_ChannelType Channel,
                   uint16 Psc, uint16 Arr);

/**
 * @brief  Set the PWM duty cycle as a percentage (0–100).
 *
 *         Internally converts: CCR = (DutyPercent * (ARR+1)) / 100
 *         This gives a linear mapping from 0% → 0 to 100% → full period.
 *
 * @param  Timer        Timer instance
 * @param  Channel      Channel to update
 * @param  DutyPercent  0 (off) to 100 (fully on)
 */
void Timer_SetDutyCycle(Timer_InstanceType Timer, Timer_ChannelType Channel,
                        uint8 DutyPercent);

/**
 * @brief  Start the timer counter (sets CEN bit).
 */
void Timer_Start(Timer_InstanceType Timer);

/**
 * @brief  Stop the timer counter (clears CEN bit).
 */
void Timer_Stop(Timer_InstanceType Timer);


#endif //PROJECT3_CODE_TIMER_H