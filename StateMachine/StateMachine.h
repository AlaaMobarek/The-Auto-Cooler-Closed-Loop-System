//
// Created by alaa on 4/27/2026.
//

#ifndef PROJECT3_CODE_STATEMACHINE_H
#define PROJECT3_CODE_STATEMACHINE_H

#include "../STD_TYPES.h"

/* ================================================================== */
/*                     System States                                  */
/* ================================================================== */
typedef enum
{
    STATE_IDLE     = 0U,    /* Fan OFF, ADC running, T < 25 °C        */
    STATE_COOLING  = 1U,    /* Fan running proportionally, 25 ≤ T < 40*/
    STATE_OVERHEAT = 2U     /* Fan at 100%, alarm LED ON, T ≥ 40      */
} SM_StateType;

/* ================================================================== */
/*                     Temperature Thresholds (in tenths of °C)       */
/* ================================================================== */
#define SM_THRESH_COOLING_ON_TENTHS     250     /*  25.0 °C */
#define SM_THRESH_TIER2_TENTHS          300     /*  30.0 °C */
#define SM_THRESH_TIER3_TENTHS          350     /*  35.0 °C */
#define SM_THRESH_OVERHEAT_TENTHS       400     /*  40.0 °C */

/* ================================================================== */
/*                     Fan Duty Cycles (%)                            */
/* ================================================================== */
#define SM_FAN_OFF      0U
#define SM_FAN_TIER1   33U      /* 25 °C ≤ T < 30 °C */
#define SM_FAN_TIER2   66U      /* 30 °C ≤ T < 35 °C */
#define SM_FAN_FULL   100U      /* T ≥ 35 °C          */

/* ================================================================== */
/*                         API                                        */
/* ================================================================== */

/**
 * @brief  Initialize the state machine.
 *         Sets initial state to IDLE and puts the alarm LED low.
 */
void SM_Init(void);

/**
 * @brief  Process a new temperature reading through the Mealy FSM.
 *
 *         This is the heart of the system. It is called from the ADC
 *         callback (ISR context) every time a new conversion is ready.
 *
 *         It evaluates the current state + input (temperature) to:
 *           1. Determine the next state
 *           2. Calculate the correct fan duty cycle
 *           3. Apply the PWM output
 *           4. Update the LCD display
 *           5. Control the alarm LED
 *
 * @param  TempTenths  Temperature in tenths of °C (e.g., 253 = 25.3 °C)
 *                     Computed from: (AdcRaw * 3300 / 4095) / 10
 *                     LM35: 10 mV/°C → V_mV / 10 = °C → ×10 for tenths
 */
void SM_ProcessTemperature(sint32 TempTenths);

/**
 * @brief  Return the current FSM state (for debugging/logging).
 */
SM_StateType SM_GetCurrentState(void);


#endif //PROJECT3_CODE_STATEMACHINE_H