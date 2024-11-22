/*
 * keypad.h
 *
 *  Created on: Oct 11, 2024
 *      Author: ArgieCS
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "stm32f4xx_hal.h"

/* Define ROW and COLUMN numbers */
#define ROWS 4
#define COLS 4

/* GPIO Ports for rows and columns */
#define KEYPAD_ROW_PORT GPIOC   // Assuming rows are connected to GPIOC
#define KEYPAD_COL_PORT GPIOE   // Assuming columns are connected to GPIOE

/* GPIO Pin definitions for rows */
#define KEYPAD_ROW_0 GPIO_PIN_0   // Adjust these pins based on your actual setup
#define KEYPAD_ROW_1 GPIO_PIN_1
#define KEYPAD_ROW_2 GPIO_PIN_2
#define KEYPAD_ROW_3 GPIO_PIN_3

/* GPIO Pin definitions for columns */
#define KEYPAD_COL_0 GPIO_PIN_2   // Adjust these pins based on your actual setup
#define KEYPAD_COL_1 GPIO_PIN_3
#define KEYPAD_COL_2 GPIO_PIN_4
#define KEYPAD_COL_3 GPIO_PIN_5

/* Function prototypes */
void Keypad_Init(void);        // Initialize the keypad GPIO
char Keypad_GetKey(void);      // Get the key pressed from the keypad

#endif /* KEYPAD_H_ */
