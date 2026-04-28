//
// Created by alaa on 4/27/2026.
//

#ifndef PROJECT3_CODE_GPIO
#define PROJECT3_CODE_GPIO

#include "../STD_TYPES.h"

/* ================================================================== */
/*                     Port Identifiers                               */
/* ================================================================== */
typedef enum
{
    GPIO_PORT_A = 0U,
    GPIO_PORT_B = 1U,
    GPIO_PORT_C = 2U,
    GPIO_PORT_D = 3U,
    GPIO_PORT_E = 4U,
    GPIO_PORT_H = 5U
} Gpio_PortType;

/* ================================================================== */
/*                     Pin Identifiers  (0-15)                        */
/* ================================================================== */
typedef enum
{
    GPIO_PIN_0  =  0U,
    GPIO_PIN_1  =  1U,
    GPIO_PIN_2  =  2U,
    GPIO_PIN_3  =  3U,
    GPIO_PIN_4  =  4U,
    GPIO_PIN_5  =  5U,
    GPIO_PIN_6  =  6U,
    GPIO_PIN_7  =  7U,
    GPIO_PIN_8  =  8U,
    GPIO_PIN_9  =  9U,
    GPIO_PIN_10 = 10U,
    GPIO_PIN_11 = 11U,
    GPIO_PIN_12 = 12U,
    GPIO_PIN_13 = 13U,
    GPIO_PIN_14 = 14U,
    GPIO_PIN_15 = 15U
} Gpio_PinType;

/* ================================================================== */
/*                     Pin Mode                                       */
/* ================================================================== */
typedef enum
{
    GPIO_MODE_INPUT     = 0U,   /* General-purpose input                   */
    GPIO_MODE_OUTPUT    = 1U,   /* General-purpose output                  */
    GPIO_MODE_AF        = 2U,   /* Alternate function                      */
    GPIO_MODE_ANALOG    = 3U    /* Analog (used for ADC/DAC channels)      */
} Gpio_ModeType;

/* ================================================================== */
/*                     Output Type                                    */
/* ================================================================== */
typedef enum
{
    GPIO_OTYPE_PUSH_PULL  = 0U, /* Push-pull  (default, drives 0 and 1)    */
    GPIO_OTYPE_OPEN_DRAIN = 1U  /* Open-drain (needs external pull-up)     */
} Gpio_OutputTypeType;

/* ================================================================== */
/*                     Output Speed                                   */
/* ================================================================== */
typedef enum
{
    GPIO_SPEED_LOW       = 0U,
    GPIO_SPEED_MEDIUM    = 1U,
    GPIO_SPEED_HIGH      = 2U,
    GPIO_SPEED_VERY_HIGH = 3U
} Gpio_SpeedType;

/* ================================================================== */
/*                     Pull-up / Pull-down                            */
/* ================================================================== */
typedef enum
{
    GPIO_PUPD_NONE     = 0U,    /* Floating                                */
    GPIO_PUPD_PULLUP   = 1U,    /* Internal pull-up resistor               */
    GPIO_PUPD_PULLDOWN = 2U     /* Internal pull-down resistor             */
} Gpio_PupdType;

/* ================================================================== */
/*                     Pin Logic Level                                */
/* ================================================================== */
typedef enum
{
    GPIO_PIN_LOW  = 0U,
    GPIO_PIN_HIGH = 1U
} Gpio_PinLevelType;

/* ================================================================== */
/*                     Pin Configuration struct                       */
/*  Pass one of these to Gpio_ConfigPin() to set up a pin in one call */
/* ================================================================== */
typedef struct
{
    Gpio_PortType       Port;
    Gpio_PinType        Pin;
    Gpio_ModeType       Mode;
    Gpio_OutputTypeType OutputType;
    Gpio_SpeedType      Speed;
    Gpio_PupdType       PullUpDown;
    uint8               AlternateFunction; /* Only used when Mode == GPIO_MODE_AF */
} Gpio_PinConfigType;

/* ================================================================== */
/*                         API                                        */
/* ================================================================== */

/**
 * @brief  Configure a single GPIO pin in one call.
 *         Covers mode, output type, speed, pull and AF.
 *         NOTE: RCC clock for the port must be enabled before calling this.
 * @param  Config  Pointer to a filled Gpio_PinConfigType structure.
 */
void Gpio_ConfigPin(const Gpio_PinConfigType *Config);

/**
 * @brief  Set pin mode only (INPUT / OUTPUT / AF / ANALOG).
 */
void Gpio_SetPinMode(Gpio_PortType Port, Gpio_PinType Pin, Gpio_ModeType Mode);

/**
 * @brief  Set output type: push-pull or open-drain.
 *         Only meaningful when pin is in OUTPUT or AF mode.
 */
void Gpio_SetOutputType(Gpio_PortType Port, Gpio_PinType Pin, Gpio_OutputTypeType OType);

/**
 * @brief  Set output speed.
 *         Only meaningful when pin is in OUTPUT or AF mode.
 */
void Gpio_SetSpeed(Gpio_PortType Port, Gpio_PinType Pin, Gpio_SpeedType Speed);

/**
 * @brief  Set pull-up / pull-down resistor.
 */
void Gpio_SetPullUpDown(Gpio_PortType Port, Gpio_PinType Pin, Gpio_PupdType Pupd);

/**
 * @brief  Set alternate function number (AF0–AF15) for a pin.
 *         Only meaningful when Mode == GPIO_MODE_AF.
 */
void Gpio_SetAlternateFunction(Gpio_PortType Port, Gpio_PinType Pin, uint8 AfNumber);

/**
 * @brief  Write a logic level to an output pin.
 *         Uses BSRR for atomic, read-modify-write-free operation.
 */
void Gpio_WritePin(Gpio_PortType Port, Gpio_PinType Pin, Gpio_PinLevelType Level);

/**
 * @brief  Toggle an output pin.
 */
void Gpio_TogglePin(Gpio_PortType Port, Gpio_PinType Pin);

/**
 * @brief  Read the current logic level of a pin (input or output).
 * @return GPIO_PIN_LOW or GPIO_PIN_HIGH
 */
Gpio_PinLevelType Gpio_ReadPin(Gpio_PortType Port, Gpio_PinType Pin);

/**
 * @brief  Write a full 16-bit value to the ODR of a port.
 *         NOTE: This writes all 16 pins at once — use carefully.
 */
void Gpio_WritePort(Gpio_PortType Port, uint16 Value);

/**
 * @brief  Read the full 16-bit IDR of a port.
 */
uint16 Gpio_ReadPort(Gpio_PortType Port);

#endif //PROJECT3_CODE_GPIO