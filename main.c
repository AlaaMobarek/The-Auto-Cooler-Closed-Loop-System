
#include "./Rcc/Rcc.h"
#include "./Gpio/Gpio.h"
#include "./Adc/Adc.h"
#include "./Timer/Timer.h"
#include "./Lcd/Lcd.h"
#include "./StateMachine/StateMachine.h"

/* ================================================================== */
/*  ADC / LM35 Conversion                                             */
/*                                                                     */
/*  LM35 output: 10 mV per °C.                                        */
/*  VREF = 3.3 V, ADC resolution = 12-bit (4096 steps).              */
/*                                                                     */
/*  V_mV  = (AdcRaw * 3300) / 4095                                    */
/*  T_°C  = V_mV / 10                                                 */
/*  T_tenths = V_mV   (because V_mV / 10 * 10 = V_mV)                */
/*                                                                     */
/*  Example: AdcRaw = 310                                             */
/*    V_mV = 310 * 3300 / 4095 ≈ 249.9 mV → T ≈ 24.9 °C             */
/*    T_tenths = 249  → "24.9 °C"                                     */
/*                                                                     */
/*  ⚠️  Integer arithmetic: multiply first, then divide, to keep      */
/*      precision. (AdcRaw * 3300) fits in uint32 up to AdcRaw=4095.  */
/* ================================================================== */
#define ADC_VREF_MV         3300U
#define ADC_MAX_COUNT       4095U

static sint32 Adc_ToTempTenths(uint16 AdcRaw)
{
    uint32 V_mV = ((uint32)AdcRaw * ADC_VREF_MV) / ADC_MAX_COUNT;
    /*
     * V_mV directly equals T_tenths because:
     *   T_°C = V_mV / 10  →  T_tenths = T_°C * 10 = V_mV
     */
    return (sint32)V_mV;
}

/* ================================================================== */
/*  ADC Callback  (called from ISR context — keep it short!)          */
/*                                                                     */
/*  This is registered with Adc_ReadSingleChannelAsync().             */
/*  When ADC completes a conversion, the ISR fires, reads DR,         */
/*  and calls this function with the raw 12-bit result.               */
/*                                                                     */
/*  We convert raw → temperature → feed to state machine.            */
/*  Then we restart the conversion for the next sample.               */
/* ================================================================== */
static void Adc_ConversionDone(uint16 AdcRaw)
{
    sint32 TempTenths = Adc_ToTempTenths(AdcRaw);
    SM_ProcessTemperature(TempTenths);

    /*
     * Restart the ADC for the next one-shot conversion.
     * Because we configured OneShot (not Continuous) mode,
     * each conversion must be manually triggered.
     */
    Adc_StartConversion();
}

/* ================================================================== */
/*  GPIO Initialization  — configure all pins used in this project    */
/* ================================================================== */
static void App_InitGpio(void)
{
    Gpio_PinConfigType PinCfg;

    /* ---- ADC input: PA1 (ADC1_IN1) — must be Analog mode ---- */
    PinCfg.Port            = GPIO_PORT_A;
    PinCfg.Pin             = GPIO_PIN_1;
    PinCfg.Mode            = GPIO_MODE_ANALOG;
    PinCfg.OutputType      = GPIO_OTYPE_PUSH_PULL;  /* don't care for analog */
    PinCfg.Speed           = GPIO_SPEED_LOW;         /* don't care for analog */
    PinCfg.PullUpDown      = GPIO_PUPD_NONE;         /* no pull for ADC input */
    PinCfg.AlternateFunction = 0U;
    Gpio_ConfigPin(&PinCfg);

    /* ---- PWM output: PA7 (TIM3_CH2, AF2) ---- */
    PinCfg.Port              = GPIO_PORT_A;
    PinCfg.Pin               = GPIO_PIN_7;
    PinCfg.Mode              = GPIO_MODE_AF;
    PinCfg.OutputType        = GPIO_OTYPE_PUSH_PULL;
    PinCfg.Speed             = GPIO_SPEED_HIGH;
    PinCfg.PullUpDown        = GPIO_PUPD_NONE;
    PinCfg.AlternateFunction = 2U;    /* AF2 = TIM3 on PA7 */
    Gpio_ConfigPin(&PinCfg);

    /* ---- LCD Control: PA8 (RS), PA9 (EN) ---- */
    PinCfg.Mode              = GPIO_MODE_OUTPUT;
    PinCfg.OutputType        = GPIO_OTYPE_PUSH_PULL;
    PinCfg.Speed             = GPIO_SPEED_MEDIUM;
    PinCfg.PullUpDown        = GPIO_PUPD_NONE;
    PinCfg.AlternateFunction = 0U;

    PinCfg.Pin = GPIO_PIN_8; Gpio_ConfigPin(&PinCfg);  /* RS */
    PinCfg.Pin = GPIO_PIN_9; Gpio_ConfigPin(&PinCfg);  /* EN */

    /* ---- LCD Data (4-bit): PB0, PB1, PB2, PB3 ---- */
    PinCfg.Port = GPIO_PORT_B;
    PinCfg.Pin = GPIO_PIN_0; Gpio_ConfigPin(&PinCfg);
    PinCfg.Pin = GPIO_PIN_1; Gpio_ConfigPin(&PinCfg);
    PinCfg.Pin = GPIO_PIN_2; Gpio_ConfigPin(&PinCfg);
    PinCfg.Pin = GPIO_PIN_3; Gpio_ConfigPin(&PinCfg);

    /* ---- Alarm LED: PC13 ---- */
    PinCfg.Port  = GPIO_PORT_C;
    PinCfg.Pin   = GPIO_PIN_13;
    PinCfg.Speed = GPIO_SPEED_LOW;
    Gpio_ConfigPin(&PinCfg);
}

/* ================================================================== */
/*                         main()                                     */
/* ================================================================== */
int main(void)
{
    /* -------------------------------------------------------------- */
    /* 1. System clock                                                  */
    /* -------------------------------------------------------------- */
    Rcc_InitSysClock();   /* HSI 16 MHz — safe for Proteus simulation  */

    /* -------------------------------------------------------------- */
    /* 2. Enable peripheral clocks                                      */
    /*    Order: GPIO ports first, then peripherals that sit on them   */
    /* -------------------------------------------------------------- */
    Rcc_EnableClock(RCC_PERIPHERAL_GPIOA);
    Rcc_EnableClock(RCC_PERIPHERAL_GPIOB);
    Rcc_EnableClock(RCC_PERIPHERAL_GPIOC);
    Rcc_EnableClock(RCC_PERIPHERAL_ADC1);
    Rcc_EnableClock(RCC_PERIPHERAL_TIM3);

    /* -------------------------------------------------------------- */
    /* 3. Configure GPIO pins                                           */
    /* -------------------------------------------------------------- */
    App_InitGpio();

    /* -------------------------------------------------------------- */
    /* 4. Initialize LCD                                                */
    /* -------------------------------------------------------------- */
    Lcd_Init();
    Lcd_SetCursor(LCD_LINE_1, 0U);
    Lcd_Print("Temp: --.- C");
    Lcd_SetCursor(LCD_LINE_2, 0U);
    Lcd_Print("Fan: 0%");

    /* -------------------------------------------------------------- */
    /* 5. Initialize PWM (TIM3, CH2, 1 kHz, 16-bit counter)            */
    /*    PSC=15 → timer ticks at 1 MHz                                */
    /*    ARR=999 → period = 1000 ticks = 1 ms → frequency = 1 kHz    */
    /* -------------------------------------------------------------- */
    Timer_PwmInit(TIMER_3, TIMER_CHANNEL_2, 15U, 999U);

    /* -------------------------------------------------------------- */
    /* 6. Initialize state machine                                      */
    /* -------------------------------------------------------------- */
    SM_Init();

    /* -------------------------------------------------------------- */
    /* 7. Initialize and start ADC                                      */
    /*    One-shot, interrupt-driven, 12-bit, channel 1 (PA1 = LM35)  */
    /* -------------------------------------------------------------- */
    Adc_Init(ADC_RES_12BIT);
    Adc_ConfigSingleChannel_OneShot(ADC_CHANNEL_1);

    /*
     * Register our callback and enable the EOC interrupt.
     * From this point, every completed ADC conversion triggers
     * ADC_IRQHandler → Adc_ConversionDone → SM_ProcessTemperature.
     */
    Adc_ReadSingleChannelAsync(Adc_ConversionDone);

    /* Trigger the very first conversion manually */
    Adc_StartConversion();

    /* -------------------------------------------------------------- */
    /* 8. Main loop — all work is done in the ADC ISR callback         */
    /* -------------------------------------------------------------- */
    while (1)
    {
        /*
         * The CPU is idle here. The system is entirely interrupt-driven:
         *   ADC EOC interrupt → Adc_ConversionDone() →
         *   SM_ProcessTemperature() → SetFan() + UpdateLcd() →
         *   Adc_StartConversion() → repeat
         *
         * You could add a low-power WFI instruction here on real hardware
         * to save power between conversions:
         *   __WFI();
         *
         * In Proteus, keep the plain while(1) — WFI may stall simulation.
         */
    }
}