/*
 * lcd.h
 *
 *  Created on: Oct 11, 2024
 *      Author: ArgieCS
 */

#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

/* Define the LCD pins */
#define LCD_RS_PIN    GPIO_PIN_0
#define LCD_E_PIN     GPIO_PIN_1
#define LCD_D4_PIN    GPIO_PIN_4
#define LCD_D5_PIN    GPIO_PIN_5
#define LCD_D6_PIN    GPIO_PIN_6
#define LCD_D7_PIN    GPIO_PIN_7
#define LCD_GPIO_PORT GPIOA

/* LCD instructions */
#define LCD_CLEAR             0x01
#define LCD_RETURN_HOME       0x02
#define LCD_ENTRY_MODE_SET    0x06
#define LCD_DISPLAY_ON        0x0C
#define LCD_DISPLAY_OFF       0x08
#define LCD_CURSOR_ON         0x0E
#define LCD_CURSOR_OFF        0x0C
#define LCD_FUNCTION_SET      0x28  // 4-bit mode, 2 lines, 5x8 dots

/* Custom character locations (CGRAM addresses 0x00 to 0x07) */
#define LCD_CHAR_BELL 0x00  // Location for bell icon
#define LCD_CHAR_PLAY 0x01  // Location for play icon
#define LCD_CHAR_PAUSE 0x02 // Location for pause icon
#define LCD_CHAR_FLAG 0x03 // Location for flag icon
#define LCD_CHAR_RESET 0x04 // Location for reset icon
#define LCD_CHAR_MODE 0x05 // Location for reset icon

void LCD_Init(void);
void LCD_Send_Cmd(uint8_t cmd);
void LCD_Send_Data(uint8_t data);
void LCD_Send_String(const char *str);
void LCD_Clear(void);
void LCD_Set_Cursor(uint8_t row, uint8_t col); // New function declaration
void LCD_Create_Custom_Char(uint8_t location, uint8_t *charmap);

#endif
