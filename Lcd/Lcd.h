//
// Created by alaa on 4/27/2026.
//

#ifndef PROJECT3_CODE_LCD_H
#define PROJECT3_CODE_LCD_H

#include "../STD_TYPES.h"

/* ================================================================== */
/*  LCD1602 Pin-to-GPIO Mapping  — change these to match your wiring  */
/*                                                                     */
/*  4-bit mode uses only D4-D7 of the LCD (lower nibble ignored).     */
/*                                                                     */
/*  Control pins:                                                      */
/*    RS  → PA8   (0 = Command, 1 = Data)                             */
/*    RW  → GND   (we always write, so tie RW low)                    */
/*    EN  → PA9   (falling edge latches data into LCD)                */
/*                                                                     */
/*  Data pins (4-bit mode):                                            */
/*    D4  → PB0                                                        */
/*    D5  → PB1                                                        */
/*    D6  → PB2                                                        */
/*    D7  → PB3                                                        */
/*                                                                     */
/*  ⚠️  RW is tied to GND in hardware. Do NOT drive it from GPIO.     */
/*      This avoids bus contention and simplifies the driver.         */
/* ================================================================== */

/* Control pin port and pin numbers */
#define LCD_CTRL_PORT       GPIO_PORT_A
#define LCD_RS_PIN          GPIO_PIN_8
#define LCD_EN_PIN          GPIO_PIN_9

/* Data pin port and pin numbers */
#define LCD_DATA_PORT       GPIO_PORT_B
#define LCD_D4_PIN          GPIO_PIN_0
#define LCD_D5_PIN          GPIO_PIN_1
#define LCD_D6_PIN          GPIO_PIN_2
#define LCD_D7_PIN          GPIO_PIN_3

/* ================================================================== */
/*  LCD Line Identifiers                                               */
/* ================================================================== */
#define LCD_LINE_1          0U
#define LCD_LINE_2          1U

/* ================================================================== */
/*  LCD1602 DDRAM Addresses                                            */
/* ================================================================== */
#define LCD_LINE1_ADDR      0x00U   /* Start of line 1 in DDRAM       */
#define LCD_LINE2_ADDR      0x40U   /* Start of line 2 in DDRAM       */

/* ================================================================== */
/*                         API                                        */
/* ================================================================== */

/**
 * @brief  Initialize the LCD in 4-bit mode.
 *         Must be called after GPIO clocks and pin config are set up.
 *         Follows the HD44780 initialization sequence from the datasheet.
 *
 *  ⚠️  This function uses blocking delay loops. Since we have no SysTick
 *      driver, delays are implemented as busy-wait loops calibrated for
 *      16 MHz (HSI). If you change the clock, re-tune the delay counts.
 */
void Lcd_Init(void);

/**
 * @brief  Send a command byte to the LCD (RS = 0).
 * @param  Command  HD44780 command byte (e.g., 0x01 = clear display)
 */
void Lcd_SendCommand(uint8 Command);

/**
 * @brief  Send a data byte (character) to the LCD (RS = 1).
 * @param  Data  ASCII character code
 */
void Lcd_SendData(uint8 Data);

/**
 * @brief  Print a null-terminated string starting at the current cursor.
 * @param  Str  Pointer to the string to display
 */
void Lcd_Print(const char *Str);

/**
 * @brief  Move the cursor to a given row and column.
 * @param  Row  LCD_LINE_1 or LCD_LINE_2
 * @param  Col  0-based column index (0–15 for a 16-char display)
 */
void Lcd_SetCursor(uint8 Row, uint8 Col);

/**
 * @brief  Clear the entire display and return cursor to home.
 *         This takes ~1.52 ms on the HD44780 — do not call every loop
 *         iteration or you will see flicker. Instead, overwrite characters
 *         in place using Lcd_SetCursor() + Lcd_Print().
 */
void Lcd_Clear(void);

/**
 * @brief  Print a signed integer to the LCD.
 *         Converts the value to decimal and prints it.
 * @param  Value  Integer to print
 */
void Lcd_PrintInt(sint32 Value);

/**
 * @brief  Print a fixed-point temperature value.
 *         Prints as "XX.X" (one decimal place).
 *         E.g., tenths = 253 prints "25.3"
 * @param  Tenths  Temperature in tenths of a degree (e.g., 253 = 25.3 °C)
 */
void Lcd_PrintTemp(sint32 Tenths);

#endif //PROJECT3_CODE_LCD_H