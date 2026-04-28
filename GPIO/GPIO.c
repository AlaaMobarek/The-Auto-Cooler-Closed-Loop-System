//
// Created by alaa on 4/27/2026.
//

#include "GPIO.h"
#include "GPIO_PRIVATE.h"
#include "../BIT_MATH.h"

/* ================================================================== */
/*                     Internal Helper                                */
/* ================================================================== */

/**
 * @brief  Resolve a Gpio_PortType enum to its peripheral base address.
 *
 *  ⚠️  GPIOF and GPIOG do not exist on STM32F401 — only A, B, C, D, E, H.
 *      If you ever port this to a larger STM32F4, add them here.
 */
static GpioType *Gpio_GetPort(Gpio_PortType Port)
{
    GpioType *Ports[] =
    {
        (GpioType *)GPIOA_BASE_ADDR,   /* GPIO_PORT_A */
        (GpioType *)GPIOB_BASE_ADDR,   /* GPIO_PORT_B */
        (GpioType *)GPIOC_BASE_ADDR,   /* GPIO_PORT_C */
        (GpioType *)GPIOD_BASE_ADDR,   /* GPIO_PORT_D */
        (GpioType *)GPIOE_BASE_ADDR,   /* GPIO_PORT_E */
        (GpioType *)GPIOH_BASE_ADDR    /* GPIO_PORT_H */
    };
    return Ports[Port];
}

/* ================================================================== */
/*              Full Pin Configuration (one-shot setup)               */
/* ================================================================== */

void Gpio_ConfigPin(const Gpio_PinConfigType *Config)
{
    Gpio_SetPinMode      (Config->Port, Config->Pin, Config->Mode);
    Gpio_SetOutputType   (Config->Port, Config->Pin, Config->OutputType);
    Gpio_SetSpeed        (Config->Port, Config->Pin, Config->Speed);
    Gpio_SetPullUpDown   (Config->Port, Config->Pin, Config->PullUpDown);

    if (Config->Mode == GPIO_MODE_AF)
    {
        Gpio_SetAlternateFunction(Config->Port, Config->Pin,
                                  Config->AlternateFunction);
    }
}

/* ================================================================== */
/*                     Individual Setters                             */
/* ================================================================== */

void Gpio_SetPinMode(Gpio_PortType Port, Gpio_PinType Pin, Gpio_ModeType Mode)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * MODER: 2 bits per pin at positions [2n+1 : 2n].
     * Clear the 2 bits first, then write the new mode.
     */
    GPIOx->MODER &= ~(0x03UL << (Pin * 2U));
    GPIOx->MODER |=  ((uint32)Mode << (Pin * 2U));
}

void Gpio_SetOutputType(Gpio_PortType Port, Gpio_PinType Pin,
                        Gpio_OutputTypeType OType)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * OTYPER: 1 bit per pin at position n.
     */
    if (OType == GPIO_OTYPE_OPEN_DRAIN)
    {
        SET_BIT(GPIOx->OTYPER, Pin);
    }
    else
    {
        CLR_BIT(GPIOx->OTYPER, Pin);
    }
}

void Gpio_SetSpeed(Gpio_PortType Port, Gpio_PinType Pin, Gpio_SpeedType Speed)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * OSPEEDR: 2 bits per pin at positions [2n+1 : 2n].
     */
    GPIOx->OSPEEDR &= ~(0x03UL << (Pin * 2U));
    GPIOx->OSPEEDR |=  ((uint32)Speed << (Pin * 2U));
}

void Gpio_SetPullUpDown(Gpio_PortType Port, Gpio_PinType Pin, Gpio_PupdType Pupd)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * PUPDR: 2 bits per pin at positions [2n+1 : 2n].
     */
    GPIOx->PUPDR &= ~(0x03UL << (Pin * 2U));
    GPIOx->PUPDR |=  ((uint32)Pupd << (Pin * 2U));
}

void Gpio_SetAlternateFunction(Gpio_PortType Port, Gpio_PinType Pin,
                               uint8 AfNumber)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * AFR is split into AFRL (pins 0-7) and AFRH (pins 8-15).
     * Each pin gets 4 bits.
     *
     * AFRL bit position = pin * 4
     * AFRH bit position = (pin - 8) * 4
     */
    if (Pin < 8U)
    {
        GPIOx->AFRL &= ~(0x0FUL << (Pin * 4U));
        GPIOx->AFRL |=  ((uint32)AfNumber << (Pin * 4U));
    }
    else
    {
        GPIOx->AFRH &= ~(0x0FUL << ((Pin - 8U) * 4U));
        GPIOx->AFRH |=  ((uint32)AfNumber << ((Pin - 8U) * 4U));
    }
}

/* ================================================================== */
/*                     Pin Read / Write                               */
/* ================================================================== */

void Gpio_WritePin(Gpio_PortType Port, Gpio_PinType Pin, Gpio_PinLevelType Level)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * Use BSRR for an atomic set/reset — no read-modify-write, no
     * critical-section needed.
     *
     * Bits [15:0]  (BSy) — write 1 to SET   pin y
     * Bits [31:16] (BRy) — write 1 to RESET pin y
     */
    if (Level == GPIO_PIN_HIGH)
    {
        GPIOx->BSRR = GPIO_BSRR_SET(Pin);
    }
    else
    {
        GPIOx->BSRR = GPIO_BSRR_RESET(Pin);
    }
}

void Gpio_TogglePin(Gpio_PortType Port, Gpio_PinType Pin)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * Toggle via ODR XOR.
     * This is NOT atomic — if an ISR touches the same port between the
     * read and the write, the result is undefined. For ISR safety, use
     * a critical section or only call TogglePin from one context.
     */
    GPIOx->ODR ^= (1UL << Pin);
}

Gpio_PinLevelType Gpio_ReadPin(Gpio_PortType Port, Gpio_PinType Pin)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * IDR reflects the actual logic level on the pin regardless of mode.
     */
    return (Gpio_PinLevelType)((GPIOx->IDR >> Pin) & 0x01U);
}

/* ================================================================== */
/*                     Port-Level Read / Write                        */
/* ================================================================== */

void Gpio_WritePort(Gpio_PortType Port, uint16 Value)
{
    GpioType *GPIOx = Gpio_GetPort(Port);

    /*
     * Writing to ODR directly affects all 16 pins.
     * Caller is responsible for ensuring only output pins are targeted.
     *
     * ⚠️  This is not atomic for partial-port writes. If you only need
     *     to control some pins of a port, use Gpio_WritePin() instead.
     */
    GPIOx->ODR = (uint32)Value;
}

uint16 Gpio_ReadPort(Gpio_PortType Port)
{
    GpioType *GPIOx = Gpio_GetPort(Port);
    return (uint16)(GPIOx->IDR & 0xFFFFU);
}
