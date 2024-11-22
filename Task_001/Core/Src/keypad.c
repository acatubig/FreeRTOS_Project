/*
 * keypad.c
 *
 *  Created on: Oct 11, 2024
 *      Author: ArgieCS
 */

#include "keypad.h"

/* Keypad layout */
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/* Row and Column Pin Definitions */
uint16_t rowPins[ROWS] = {KEYPAD_ROW_0, KEYPAD_ROW_1, KEYPAD_ROW_2, KEYPAD_ROW_3};  // GPIO pins for rows
uint16_t colPins[COLS] = {KEYPAD_COL_0, KEYPAD_COL_1, KEYPAD_COL_2, KEYPAD_COL_3};  // GPIO pins for columns

/* Initialize GPIO for keypad */
void Keypad_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIO Clocks */
    __HAL_RCC_GPIOC_CLK_ENABLE();  // Assuming rows on GPIOC
    __HAL_RCC_GPIOE_CLK_ENABLE();  // Assuming columns on GPIOE

    /* Configure ROW pins (input, pull-up) */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    for (int i = 0; i < ROWS; i++) {
        GPIO_InitStruct.Pin = rowPins[i];
        HAL_GPIO_Init(KEYPAD_ROW_PORT, &GPIO_InitStruct);
    }

    /* Configure COLUMN pins (output, push-pull) */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    for (int i = 0; i < COLS; i++) {
        GPIO_InitStruct.Pin = colPins[i];
        HAL_GPIO_Init(KEYPAD_COL_PORT, &GPIO_InitStruct);
        HAL_GPIO_WritePin(KEYPAD_COL_PORT, colPins[i], GPIO_PIN_SET);  // Set column pins HIGH initially
    }
}

/* Get the key pressed from the keypad */
char Keypad_GetKey(void)
{
    for (int col = 0; col < COLS; col++)
    {
        // Set all columns HIGH
        for (int i = 0; i < COLS; i++) {
            HAL_GPIO_WritePin(KEYPAD_COL_PORT, colPins[i], GPIO_PIN_SET);
        }

        // Set current column LOW
        HAL_GPIO_WritePin(KEYPAD_COL_PORT, colPins[col], GPIO_PIN_RESET);

        // Check each row
        for (int row = 0; row < ROWS; row++)
        {
            if (HAL_GPIO_ReadPin(KEYPAD_ROW_PORT, rowPins[row]) == GPIO_PIN_RESET)
            {
                /* Wait for debounce */
                HAL_Delay(50);

                /* Check again */
                if (HAL_GPIO_ReadPin(KEYPAD_ROW_PORT, rowPins[row]) == GPIO_PIN_RESET)
                {
                    // Wait for key release
                    while (HAL_GPIO_ReadPin(KEYPAD_ROW_PORT, rowPins[row]) == GPIO_PIN_RESET);

                    return hexaKeys[row][col];  // Return the corresponding key
                }
            }
        }
    }

    return '\0';  // No key pressed
}
