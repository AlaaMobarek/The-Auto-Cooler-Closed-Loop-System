//
// Created by alaa on 4/27/2026.
//

#include "StateMachine.h"
#include "../Gpio/Gpio.h"
#include "../Timer/Timer.h"
#include "../Lcd/Lcd.h"

/* ================================================================== */
/*  Alarm LED pin  — change to match your wiring                      */
/* ================================================================== */
#define ALARM_LED_PORT      GPIO_PORT_C
#define ALARM_LED_PIN       GPIO_PIN_13

/* ================================================================== */
/*  PWM output — must match Timer_PwmInit() call in main.c            */
/* ================================================================== */
#define FAN_TIMER           TIMER_3
#define FAN_CHANNEL         TIMER_CHANNEL_2

/* ================================================================== */
/*  Module state                                                       */
/* ================================================================== */
static SM_StateType SM_CurrentState = STATE_IDLE;

/* ================================================================== */
/*  Internal helpers                                                   */
/* ================================================================== */

/**
 * @brief  Set fan PWM and update LCD line 2 with duty cycle.
 */
static void SM_SetFan(uint8 DutyPercent)
{
    // char DutyStr[5];
    // uint8 d = DutyPercent;

    Timer_SetDutyCycle(FAN_TIMER, FAN_CHANNEL, DutyPercent);

    /* Write "Fan: XX%" on line 2 without clearing the display */
    Lcd_SetCursor(LCD_LINE_2, 0U);
    Lcd_Print("Fan:           ");   /* overwrite full line with spaces */
    Lcd_SetCursor(LCD_LINE_2, 0U);
    Lcd_Print("Fan: ");
    Lcd_PrintInt((sint32)DutyPercent);
    Lcd_SendData('%');
}

/**
 * @brief  Display temperature on LCD line 1.
 */
static void SM_DisplayTemp(sint32 TempTenths)
{
    Lcd_SetCursor(LCD_LINE_1, 0U);
    Lcd_Print("Temp:          ");  /* overwrite full line */
    Lcd_SetCursor(LCD_LINE_1, 0U);
    Lcd_Print("Temp: ");
    Lcd_PrintTemp(TempTenths);
    Lcd_Print(" C");
}

/**
 * @brief  Compute duty cycle from temperature (for COOLING state).
 *         Uses the fixed thresholds from the project spec.
 */
static uint8 SM_CalcDuty(sint32 TempTenths)
{
    if (TempTenths >= SM_THRESH_TIER3_TENTHS)
    {
        return SM_FAN_FULL;
    }
    else if (TempTenths >= SM_THRESH_TIER2_TENTHS)
    {
        return SM_FAN_TIER2;
    }
    else
    {
        return SM_FAN_TIER1;
    }
}

/* ================================================================== */
/*                         State Handlers                             */
/*                                                                     */
/*  Each handler represents one row in the Mealy state table:         */
/*  given current state + input → produce output + next state         */
/* ================================================================== */

/**
 * @brief  Handle IDLE state.
 *         Transitions:
 *           T < 25 °C  → stay IDLE,   fan OFF
 *           T ≥ 25 °C  → go COOLING,  fan ON
 */
static SM_StateType SM_HandleIdle(sint32 TempTenths)
{
    SM_DisplayTemp(TempTenths);

    if (TempTenths >= SM_THRESH_COOLING_ON_TENTHS)
    {
        /* Output: apply fan, show % */
        uint8 Duty = SM_CalcDuty(TempTenths);
        SM_SetFan(Duty);
        return STATE_COOLING;
    }
    else
    {
        /* Output: fan stays OFF */
        SM_SetFan(SM_FAN_OFF);
        return STATE_IDLE;
    }
}

/**
 * @brief  Handle COOLING state.
 *         Transitions:
 *           T < 25 °C  → go IDLE,      fan OFF
 *           T ≥ 40 °C  → go OVERHEAT,  fan 100%, alarm ON
 *           otherwise  → stay COOLING,  recalc duty
 */
static SM_StateType SM_HandleCooling(sint32 TempTenths)
{
    SM_DisplayTemp(TempTenths);

    if (TempTenths < SM_THRESH_COOLING_ON_TENTHS)
    {
        SM_SetFan(SM_FAN_OFF);
        Gpio_WritePin(ALARM_LED_PORT, ALARM_LED_PIN, GPIO_PIN_LOW);
        return STATE_IDLE;
    }
    else if (TempTenths >= SM_THRESH_OVERHEAT_TENTHS)
    {
        SM_SetFan(SM_FAN_FULL);
        Gpio_WritePin(ALARM_LED_PORT, ALARM_LED_PIN, GPIO_PIN_HIGH);

        /* Show overheat warning on line 2 */
        Lcd_SetCursor(LCD_LINE_2, 0U);
        Lcd_Print("!! OVERHEAT !!  ");

        return STATE_OVERHEAT;
    }
    else
    {
        uint8 Duty = SM_CalcDuty(TempTenths);
        SM_SetFan(Duty);
        return STATE_COOLING;
    }
}

/**
 * @brief  Handle OVERHEAT state.
 *         Transitions:
 *           T ≥ 40 °C  → stay OVERHEAT, maintain 100%, alarm ON
 *           T < 40 °C  → go COOLING,    resume normal mapping
 */
static SM_StateType SM_HandleOverheat(sint32 TempTenths)
{
    SM_DisplayTemp(TempTenths);

    if (TempTenths >= SM_THRESH_OVERHEAT_TENTHS)
    {
        /* Maintain overheat outputs */
        SM_SetFan(SM_FAN_FULL);
        Gpio_WritePin(ALARM_LED_PORT, ALARM_LED_PIN, GPIO_PIN_HIGH);

        Lcd_SetCursor(LCD_LINE_2, 0U);
        Lcd_Print("!! OVERHEAT !!  ");

        return STATE_OVERHEAT;
    }
    else
    {
        /* Temperature dropped below 40 °C — clear alarm, resume cooling */
        Gpio_WritePin(ALARM_LED_PORT, ALARM_LED_PIN, GPIO_PIN_LOW);

        uint8 Duty = SM_CalcDuty(TempTenths);
        SM_SetFan(Duty);

        return STATE_COOLING;
    }
}

/* ================================================================== */
/*                     State Transition Table                         */
/*                                                                     */
/*  This function pointer table makes the FSM explicit and            */
/*  traceable — no flat if-else chain spanning hundreds of lines.     */
/* ================================================================== */
typedef SM_StateType (*SM_HandlerFn)(sint32);

static const SM_HandlerFn SM_StateTable[] =
{
    SM_HandleIdle,      /* STATE_IDLE     */
    SM_HandleCooling,   /* STATE_COOLING  */
    SM_HandleOverheat   /* STATE_OVERHEAT */
};

/* ================================================================== */
/*                         Public API                                 */
/* ================================================================== */

void SM_Init(void)
{
    SM_CurrentState = STATE_IDLE;
    Gpio_WritePin(ALARM_LED_PORT, ALARM_LED_PIN, GPIO_PIN_LOW);
    Timer_SetDutyCycle(FAN_TIMER, FAN_CHANNEL, SM_FAN_OFF);
}

void SM_ProcessTemperature(sint32 TempTenths)
{
    /*
     * Mealy machine evaluation:
     *   next_state = f(current_state, input)
     *   output     = g(current_state, input)    ← both inside the handler
     *
     * The handler does both: it produces the output AND returns the next state.
     */
    SM_CurrentState = SM_StateTable[SM_CurrentState](TempTenths);
}

SM_StateType SM_GetCurrentState(void)
{
    return SM_CurrentState;
}