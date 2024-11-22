/*
 * lcd.c
 *
 *  Created on: Oct 11, 2024
 *      Author: ArgieCS
 */

#include "lcd.h"

/* Custom Bell Icon (8x8 pixel array) */
uint8_t bellIcon[8] = {
    0x0E, 0x1F, 0x1F, 0x1F,
    0x1F, 0x0E, 0x04, 0x04
};

/* Custom Play Icon (8x8 pixel array) */
uint8_t playIcon[8] = {
	0b00001000,
	0b00001100,
	0b00001110,
	0b00001111,
	0b00001110,
	0b00001100,
	0b00001000,
	0b00000000
};

/* Custom Pause Icon (8x8 pixel array) */
uint8_t pauseIcon[8] = {
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00000000
};

/* Custom flag Icon (8x8 pixel array) */
uint8_t flagIcon[8] = {
	0b00010000,
	0b00011000,
	0b00011100,
	0b00011110,
	0b00011111,
	0b00010000,
	0b00010000,
	0b00000000
};

/* Custom reset Icon (8x8 pixel array) */
uint8_t resetIcon[8] = {
	0b00001101,
	0b00010011,
	0b00010111,
	0b00010000,
	0b00010001,
	0b00010001,
	0b00001110,
	0b00000000
};

/* Custom mode Icon (8x8 pixel array) */
uint8_t modeIcon[8] = {
	0b00011111,
	0b00011111,
	0b00000000,
	0b00011111,
	0b00011111,
	0b00000000,
	0b00011111,
	0b00011111
};

/* Delay function */
static void LCD_Delay(uint16_t ms)
{
    HAL_Delay(ms);
}

/* Initialize LCD */
void LCD_Init(void)
{
    /* Initialize GPIO pins */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Enable clock for GPIOA

    //Configure GPIO pin Output Level
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                              |GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = LCD_RS_PIN | LCD_E_PIN | LCD_D4_PIN | LCD_D5_PIN | LCD_D6_PIN | LCD_D7_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);

    /* Initialization sequence */
    LCD_Delay(50);
    LCD_Send_Cmd(0x03); // 8-bit mode, send three times
    LCD_Delay(5);
    LCD_Send_Cmd(0x03);
    LCD_Delay(5);
    LCD_Send_Cmd(0x03);
    LCD_Delay(5);
    LCD_Send_Cmd(0x02); // 4-bit mode
    LCD_Delay(5);

    /* Set LCD to 4-bit mode, 2-line, 5x8 font */
    LCD_Send_Cmd(LCD_FUNCTION_SET);
    LCD_Send_Cmd(LCD_DISPLAY_ON);
    LCD_Clear();
    LCD_Send_Cmd(LCD_ENTRY_MODE_SET);

    // Create custom characters (bell, play, pause)
    LCD_Create_Custom_Char(LCD_CHAR_BELL, bellIcon);
    LCD_Create_Custom_Char(LCD_CHAR_PLAY, playIcon);
    LCD_Create_Custom_Char(LCD_CHAR_PAUSE, pauseIcon);
    LCD_Create_Custom_Char(LCD_CHAR_FLAG, flagIcon);
    LCD_Create_Custom_Char(LCD_CHAR_RESET, resetIcon);
    LCD_Create_Custom_Char(LCD_CHAR_MODE, modeIcon);
}

/* Send command to LCD */
void LCD_Send_Cmd(uint8_t cmd)
{
    /* Set RS to 0 for command */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_RS_PIN, GPIO_PIN_RESET);

    /* Send higher nibble */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D4_PIN, (cmd & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D5_PIN, (cmd & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D6_PIN, (cmd & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D7_PIN, (cmd & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Trigger enable */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_SET);
    LCD_Delay(0.1);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_RESET);

    /* Send lower nibble */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D4_PIN, (cmd & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D5_PIN, (cmd & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D6_PIN, (cmd & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D7_PIN, (cmd & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Trigger enable */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_SET);
    LCD_Delay(0.1);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_RESET);
}

/* Send data to LCD */
void LCD_Send_Data(uint8_t data)
{
    /* Set RS to 1 for data */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_RS_PIN, GPIO_PIN_SET);

    /* Send higher nibble */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D4_PIN, (data & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D5_PIN, (data & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D6_PIN, (data & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D7_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Trigger enable */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_SET);
    LCD_Delay(0.1);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_RESET);

    /* Send lower nibble */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D4_PIN, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D5_PIN, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D6_PIN, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_D7_PIN, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Trigger enable */
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_SET);
    LCD_Delay(0.1);
    HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_E_PIN, GPIO_PIN_RESET);
}

/* Send a string to LCD */
void LCD_Send_String(const char *str)
{
    while (*str)
    {
        LCD_Send_Data((uint8_t)(*str));
        str++;
    }
}

/* Clear LCD */
void LCD_Clear(void)
{
    LCD_Send_Cmd(LCD_CLEAR);
    LCD_Delay(0.1); // Wait for the command to complete
}

/* Set the cursor to a specific row and column */
void LCD_Set_Cursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    switch (row)
    {
        case 0: address = 0x00 + col; break; // First row
        case 1: address = 0x40 + col; break; // Second row
        case 2: address = 0x14 + col; break; // Third row (for 20x4 LCD)
        case 3: address = 0x54 + col; break; // Fourth row (for 20x4 LCD)
        default: address = 0x00 + col; break; // Default to first row
    }

    /* Send the command to set DDRAM address */
    LCD_Send_Cmd(0x80 | address);
}

/* Create custom characters (e.g., bell, play/pause) */
void LCD_Create_Custom_Char(uint8_t location, uint8_t *charmap)
{
    location &= 0x07;  // Max 8 custom characters (0-7)
    LCD_Send_Cmd(0x40 | (location << 3));  // Set CGRAM address

    for (uint8_t i = 0; i < 8; i++)
    {
        LCD_Send_Data(charmap[i]);  // Write the custom character data to CGRAM
    }
}
