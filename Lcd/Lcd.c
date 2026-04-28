//
// Created by alaa on 4/27/2026.
//

#include "Lcd.h"
#include "../Gpio/Gpio.h"
#include "../BIT_MATH.h"

/* ================================================================== */
/*  HD44780 Command Set  (most commonly used)                         */
/* ================================================================== */
#define LCD_CMD_CLEAR           0x01U   /* Clear display, cursor home  */
#define LCD_CMD_HOME            0x02U   /* Return cursor home          */
#define LCD_CMD_ENTRY_MODE      0x06U   /* Increment cursor, no shift  */
#define LCD_CMD_DISPLAY_ON      0x0CU   /* Display ON, cursor OFF      */
#define LCD_CMD_DISPLAY_OFF     0x08U
#define LCD_CMD_FUNCTION_SET_4BIT 0x28U /* 4-bit, 2-line, 5x8 font    */
#define LCD_CMD_SET_DDRAM       0x80U   /* OR with address to set DDRAM*/

/* ================================================================== */
/*  Busy-Wait Delay (calibrated for 16 MHz HSI)                       */
/*                                                                     */
/*  At 16 MHz, one iteration of a volatile loop takes ~4 clock cycles  */
/*  = 250 ns. We use counts derived from that.                        */
/*                                                                     */
/*  ⚠️  These are rough estimates. Proteus simulation runs slower than  */
/*      real hardware. If the LCD doesn't respond in Proteus, increase  */
/*      the delay multipliers below.                                   */
/* ================================================================== */
static void Lcd_DelayUs(uint32 Us)
{
    /*
     * At 16 MHz: ~4 cycles per loop → 1 us ≈ 4 iterations.
     * Volatile prevents the compiler from optimising the loop away.
     */
    volatile uint32 Count = Us * 4U;
    while (Count > 0U)
    {
        Count--;
    }
}

static void Lcd_DelayMs(uint32 Ms)
{
    Lcd_DelayUs(Ms * 1000U);
}

/* ================================================================== */
/*  Internal: pulse EN pin to latch the nibble into the LCD           */
/* ================================================================== */
static void Lcd_PulseEnable(void)
{
    /*
     * HD44780 requires EN high for >= 450 ns, then falling edge.
     * We drive high, wait, then drive low.
     */
    Gpio_WritePin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_HIGH);
    Lcd_DelayUs(1U);
    Gpio_WritePin(LCD_CTRL_PORT, LCD_EN_PIN, GPIO_PIN_LOW);
    Lcd_DelayUs(50U);   /* Wait >= 37 us for most commands to execute */
}

/* ================================================================== */
/*  Internal: write the high nibble of a byte to D4-D7               */
/* ================================================================== */
static void Lcd_WriteNibble(uint8 Nibble)
{
    /*
     * We write each data bit individually to the GPIO pins.
     * This is slower than a port-write but avoids corrupting
     * other pins on the data port.
     *
     * Nibble bit 0 → D4, bit 1 → D5, bit 2 → D6, bit 3 → D7
     */
    Gpio_WritePin(LCD_DATA_PORT, LCD_D4_PIN,
                  (Nibble & 0x01U) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    Gpio_WritePin(LCD_DATA_PORT, LCD_D5_PIN,
                  (Nibble & 0x02U) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    Gpio_WritePin(LCD_DATA_PORT, LCD_D6_PIN,
                  (Nibble & 0x04U) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    Gpio_WritePin(LCD_DATA_PORT, LCD_D7_PIN,
                  (Nibble & 0x08U) ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
}

/* ================================================================== */
/*  Internal: send a full byte in two 4-bit nibbles (MSN first)       */
/* ================================================================== */
static void Lcd_SendByte(uint8 Byte, uint8 IsData)
{
    /* RS = 1 for data, RS = 0 for command */
    Gpio_WritePin(LCD_CTRL_PORT, LCD_RS_PIN,
                  IsData ? GPIO_PIN_HIGH : GPIO_PIN_LOW);

    /* High nibble first */
    Lcd_WriteNibble((Byte >> 4U) & 0x0FU);
    Lcd_PulseEnable();

    /* Low nibble second */
    Lcd_WriteNibble(Byte & 0x0FU);
    Lcd_PulseEnable();
}

/* ================================================================== */
/*                         Public API                                 */
/* ================================================================== */

void Lcd_SendCommand(uint8 Command)
{
    Lcd_SendByte(Command, 0U);

    /*
     * Most commands need 37 us, but Clear (0x01) and Home (0x02)
     * need 1.52 ms. We add an extra delay here for safety.
     */
    if (Command == LCD_CMD_CLEAR || Command == LCD_CMD_HOME)
    {
        Lcd_DelayMs(2U);
    }
}

void Lcd_SendData(uint8 Data)
{
    Lcd_SendByte(Data, 1U);
}

void Lcd_Init(void)
{
    /*
     * HD44780 4-bit initialization sequence (from datasheet Figure 24):
     *
     * 1. Wait >15 ms after VCC rises to 4.5V (or >40 ms after 2.7V)
     * 2. Send 0x3 in 8-bit mode (upper nibble only), wait >4.1 ms
     * 3. Send 0x3 again, wait >100 us
     * 4. Send 0x3 a third time
     * 5. Send 0x2 → switches to 4-bit mode
     * 6. Now send full commands in 4-bit mode
     */

    Lcd_DelayMs(20U);   /* Power-on delay */

    /* Step 2: send 0x3 (8-bit mode) */
    Gpio_WritePin(LCD_CTRL_PORT, LCD_RS_PIN, GPIO_PIN_LOW);
    Lcd_WriteNibble(0x03U);
    Lcd_PulseEnable();
    Lcd_DelayMs(5U);

    /* Step 3: send 0x3 again */
    Lcd_WriteNibble(0x03U);
    Lcd_PulseEnable();
    Lcd_DelayUs(150U);

    /* Step 4: send 0x3 third time */
    Lcd_WriteNibble(0x03U);
    Lcd_PulseEnable();

    /* Step 5: switch to 4-bit mode */
    Lcd_WriteNibble(0x02U);
    Lcd_PulseEnable();

    /* Step 6: configure in 4-bit mode */
    Lcd_SendCommand(LCD_CMD_FUNCTION_SET_4BIT); /* 4-bit, 2 lines, 5x8 */
    Lcd_SendCommand(LCD_CMD_DISPLAY_ON);         /* Display on, cursor off */
    Lcd_SendCommand(LCD_CMD_CLEAR);              /* Clear display           */
    Lcd_SendCommand(LCD_CMD_ENTRY_MODE);         /* Auto-increment cursor   */

    Lcd_DelayMs(2U);
}

void Lcd_SetCursor(uint8 Row, uint8 Col)
{
    uint8 Addr;

    if (Row == LCD_LINE_1)
    {
        Addr = LCD_LINE1_ADDR + Col;
    }
    else
    {
        Addr = LCD_LINE2_ADDR + Col;
    }

    /* Set DDRAM address command: bit7 = 1, bits6:0 = address */
    Lcd_SendCommand(LCD_CMD_SET_DDRAM | Addr);
}

void Lcd_Print(const char *Str)
{
    while (*Str != '\0')
    {
        Lcd_SendData((uint8)(*Str));
        Str++;
    }
}

void Lcd_Clear(void)
{
    Lcd_SendCommand(LCD_CMD_CLEAR);
}

void Lcd_PrintInt(sint32 Value)
{
    char    Buf[12];
    uint8   Idx  = 0U;
    uint8   Start;
    uint8   End;
    char    Tmp;
    boolean Negative = FALSE;

    if (Value < 0)
    {
        Negative = TRUE;
        Value    = -Value;
        Lcd_SendData('-');
    }

    if (Value == 0)
    {
        Lcd_SendData('0');
        return;
    }

    /* Build digits in reverse */
    while (Value > 0)
    {
        Buf[Idx++] = (char)('0' + (Value % 10));
        Value /= 10;
    }

    /* Reverse the buffer */
    Start = 0U;
    End   = Idx - 1U;
    while (Start < End)
    {
        Tmp        = Buf[Start];
        Buf[Start] = Buf[End];
        Buf[End]   = Tmp;
        Start++;
        End--;
    }
    Buf[Idx] = '\0';

    Lcd_Print(Buf);

    (void)Negative; /* suppress unused warning if negative handled above */
}

void Lcd_PrintTemp(sint32 Tenths)
{
    /*
     * Tenths = temperature × 10, e.g. 253 = 25.3 °C
     * We print: integer part, '.', decimal part
     */
    sint32 IntPart  = Tenths / 10;
    sint32 FracPart = Tenths % 10;

    if (FracPart < 0) { FracPart = -FracPart; }

    Lcd_PrintInt(IntPart);
    Lcd_SendData('.');
    Lcd_SendData((uint8)('0' + FracPart));
}