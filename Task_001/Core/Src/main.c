/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lcd.h"
#include "keypad.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define min(a, b) ((a) < (b) ? (a) : (b))

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
TaskHandle_t lcdTaskHandle;
TaskHandle_t keypadTaskHandle;
TaskHandle_t chronoTaskHandle;
TaskHandle_t timekeepingTaskHandle;
QueueHandle_t xQueue;  // Queue for sending keypress to the LCD task
RTC_HandleTypeDef hrtc;
enum Mode { TOD, SET_TIME_DATE, CHRONO } currentMode = TOD; // Mode tracking
bool chronoRunning = false;

// Time Variables
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

char buffer[22];
uint32_t elapsedTime = 0, startTime = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void set_time(void);
static void task_LCD(void* parameters);
static void task_Keypad(void* parameters);
static void task_Chronograph(void* parameters);
static void vTask_Timekeeping(void* parameters);
int calculate_weekday(int day, int month, int year);
void displayTOD(void);
void displaySetTimeDate(void);
void displayChrono(void);
int getMaxDaysInMonth(int month, int year);
void setTodTime(RTC_TimeTypeDef syncTime, RTC_DateTypeDef syncDate);
// Constants for day names
const char *weekday[] = {"Mon", "Tue", "Wed", "Thu", "Fri","Sat", "Sun"};
// Static variables to track changes in time and date
static int prevDate = -1, prevMonth = -1, prevYear = -1;
static int prevHour = -1, prevMinute = -1, prevSecond = -1;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  // Initialize the LCD
  LCD_Init();
  //LCD_Send_String("Hello, World!");

  // Create a queue for communication between tasks
  xQueue = xQueueCreate(5, sizeof(char));  // Queue to hold keypresses
  configASSERT(xQueue != NULL);


  xTaskCreate(task_LCD, "LCD_Task", 200, NULL, 1, &lcdTaskHandle);
  xTaskCreate(task_Keypad, "Keypad_Task", 200, NULL, 1, &keypadTaskHandle);
  xTaskCreate(vTask_Timekeeping, "Timekeeping_Task", 200, NULL, 1, &timekeepingTaskHandle);
  xTaskCreate(task_Chronograph, "Chrono_Task", 200, NULL, 1, &chronoTaskHandle);


  //start the FreeRTOS scheduler
  vTaskStartScheduler();

  //if the control comes here, then the launch of the scheduler has failed due to insufficient memory in heap
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  sDate.Month = RTC_MONTH_OCTOBER;
  sDate.Date = 0x16;
  sDate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA4 PA5 PA6
                           PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* Timekeeping Task */
void vTask_Timekeeping(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);  // 1-second interval

    for (;;) {
        // Update current time and date values from the RTC every second
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        // Signal the display task to update the time display
        xTaskNotifyGive(lcdTaskHandle);

        // Delay until the next second
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}


/* LCD Task */
static void task_LCD(void* parameters) {
	while (1) {
		switch (currentMode) {
			case TOD:
				displayTOD();
				break;
			case SET_TIME_DATE:
				displaySetTimeDate();
				break;
			case CHRONO:
				displayChrono();
				break;
		}
		//vTaskDelay(pdMS_TO_TICKS(500));
	}
}


/* Keypad Task */
static void task_Keypad(void* parameters){
	char key;
	while (1) {
		key = Keypad_GetKey();
		if (key != '\0') {
			xQueueSend(xQueue, &key, portMAX_DELAY);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

// Chronograph Task: Manages chrono timing
static void task_Chronograph(void* parameters) {
    while (1) {
        if (chronoRunning) {
            elapsedTime = HAL_GetTick() - startTime;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Static variable to track if the TOD display needs full refresh
static bool isTodInitialized = false;

void displayTOD(void) {
	// Wait for notification from the timekeeping task
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	// Array of weekday names
	char *weekday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	int currentWeekday = calculate_weekday(sDate.Date, sDate.Month, 2000 + sDate.Year);

	// Force an update of all fields if initializing
	if (!isTodInitialized) {
		prevDate = -1;  // Force date to update
		prevMonth = -1;
		prevYear = -1;
		prevHour = -1;
		prevMinute = -1;
		prevSecond = -1;
		isTodInitialized = true;
	}

	// 1. Update Date if it Changes
	if (sDate.Date != prevDate || sDate.Month != prevMonth || sDate.Year != prevYear) {
		sprintf(buffer, "   %s %02d/%02d/%04d   ", weekday[currentWeekday], sDate.Month, sDate.Date, 2000 + sDate.Year);
		LCD_Set_Cursor(0, 0);
		LCD_Send_String(buffer);

		// Store the new date values
		prevDate = sDate.Date;
		prevMonth = sDate.Month;
		prevYear = sDate.Year;
	}

	// 2. Update Hour and Minute if they Change
	int currentHour = sTime.Hours % 12 == 0 ? 12 : sTime.Hours % 12;
	if (currentHour != prevHour || sTime.Minutes != prevMinute) {
		sprintf(buffer, "    %02d:%02d %s    ", currentHour, sTime.Minutes, sTime.Hours < 12 ? "AM" : "PM");
		LCD_Set_Cursor(1, 0);
		LCD_Send_String(buffer);

		// Store the new hour and minute values
		prevHour = currentHour;
		prevMinute = sTime.Minutes;
	}

	// 3. Update Seconds Every Second
	if (sTime.Seconds != prevSecond) {
		sprintf(buffer, "    %02d:%02d:%02d %s    ", currentHour, sTime.Minutes, sTime.Seconds, sTime.Hours < 12 ? "AM" : "PM");
		LCD_Set_Cursor(1, 6);  // Update only the seconds part of the time
		LCD_Send_String(buffer + 6); // Send only the seconds part

		// Store the new second value
		prevSecond = sTime.Seconds;
	}

	// Display static control options only once
	// Display control options (once only)
	LCD_Set_Cursor(2, 0);
	LCD_Send_String("[A]Set    [C]Indiglo");
	LCD_Set_Cursor(3, 0);
	LCD_Send_String("[B]Mode   [D]Stp/Rst");

	// Handle user input for mode switching
	char key;
	if (xQueueReceive(xQueue, &key, 0)) {
		switch (key) {
			case 'A': currentMode = SET_TIME_DATE; break;
			case 'B': currentMode = CHRONO; break;
			case 'D':
				chronoRunning = !chronoRunning;
				if (chronoRunning) startTime = HAL_GetTick() - elapsedTime;
				break;
		}
	}

	// No need for delay here; waiting is handled by the task notification

}


// Display Set Time and Date Function
void displaySetTimeDate(void) {
    RTC_TimeTypeDef newTime;
    RTC_DateTypeDef newDate;

    // Initialize newTime and newDate to the current time and date
    HAL_RTC_GetTime(&hrtc, &newTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &newDate, RTC_FORMAT_BIN);

    char buffer[64];
    char receivedKey;
    int editingField = 6;  // 0: hour, 1: minute, 2: second, 3: day, 4: month, 5: year, 6: format
    static int timeFormat = 1; // 1 for 24-hour format, 0 for 12-hour format
    TickType_t lastKeyPressTime = xTaskGetTickCount();  // Track last key press for timeout

    LCD_Clear();

    while (1) {
    	// Fetch current time for updating seconds display
		RTC_TimeTypeDef currentTime;
		HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);

        // Auto-exit if idle for 10 seconds
        if (xTaskGetTickCount() - lastKeyPressTime > pdMS_TO_TICKS(10000)) {
            currentMode = TOD;
            isTodInitialized = false;
            break;
        }

        // Display the time with live seconds update
		int displayHours = newTime.Hours;
		char *ampm = "";

		if (timeFormat == 0) {  // 12-hour format
			ampm = "AM";
			if (displayHours >= 12) {
				ampm = "PM";
				if (displayHours > 12) {
					displayHours -= 12;
				}
			} else if (displayHours == 0) {
				displayHours = 12;  // Midnight case
			}
			sprintf(buffer, "Time: %02d:%02d:%02d %s", displayHours, newTime.Minutes, currentTime.Seconds, ampm);
		} else {  // 12-hour format
			sprintf(buffer, "Time: %02d:%02d:%02d   ", newTime.Hours, newTime.Minutes, currentTime.Seconds);
		}

        LCD_Set_Cursor(0, 0);
        LCD_Send_String(buffer); // Update whole time display

        // Display Date
		sprintf(buffer, "Date: %02d/%02d/%04d", newDate.Month, newDate.Date, 2000 + newDate.Year);
		LCD_Set_Cursor(1, 0);
		LCD_Send_String(buffer);

        LCD_Set_Cursor(2, 0);
        sprintf(buffer, "[B]%s     [C]++",
                editingField == 0 ? "Hour  " :
                editingField == 1 ? "Min   " :
                editingField == 2 ? "Sec   " :
                editingField == 3 ? "Day   " :
                editingField == 4 ? "Month " :
                editingField == 5 ? "Year  " : "Format");
        LCD_Send_String(buffer);

        LCD_Set_Cursor(3, 0);
        LCD_Send_String("[A]Done       [D]--");

        // Handle Keypresses
        if (xQueueReceive(xQueue, &receivedKey, 0)) {
            lastKeyPressTime = xTaskGetTickCount();  // Reset timeout on key press

            if (receivedKey == 'A') {
                // Save the time and date, and exit the function
                LCD_Clear();
                // Before saving, store the current running second in newTime.Seconds
                newTime.Seconds = currentTime.Seconds; // Save the running seconds value

                newDate.WeekDay = calculate_weekday(newDate.Date, newDate.Month, 2000 + newDate.Year);
                HAL_RTC_SetDate(&hrtc, &newDate, RTC_FORMAT_BIN);
                HAL_RTC_SetTime(&hrtc, &newTime, RTC_FORMAT_BIN);

                currentMode = TOD;
                isTodInitialized = false;  // Reset TOD initialization flag

                // Optional: Signal the timekeeping task to refresh the TOD display
                xTaskNotifyGive(lcdTaskHandle);
                break; // Exit the time-setting mode

            } else if (receivedKey == 'B') { // Next field
                editingField = (editingField + 1) % 7;
            } else if (receivedKey == 'C' || receivedKey == 'D') { // Increase or decrease current field
                int increment = (receivedKey == 'C') ? 1 : -1;

                if (editingField == 0) {  // Hour
                    newTime.Hours = (newTime.Hours + increment + 24) % 24;
                } else if (editingField == 1) {  // Minute
                    newTime.Minutes = (newTime.Minutes + increment + 60) % 60;
                } else if (editingField == 2) {  // Second
                	// Reset the seconds to zero when editing the seconds field
					newTime.Seconds = 0;
					currentTime.Seconds = 0;  // Ensure running seconds are also reset to zero
					HAL_RTC_SetTime(&hrtc, &newTime, RTC_FORMAT_BIN);
                } else if (editingField == 3) {  // Day
                    int maxDays = getMaxDaysInMonth(newDate.Month, 2000 + newDate.Year);
                    newDate.Date = (newDate.Date + increment + maxDays) % maxDays;
                    if (newDate.Date == 0) newDate.Date = maxDays;
                } else if (editingField == 4) {  // Month
                    newDate.Month = (newDate.Month + increment + 12) % 12;
                    if (newDate.Month == 0) newDate.Month = 12;
                    newDate.Date = min(newDate.Date, getMaxDaysInMonth(newDate.Month, 2000 + newDate.Year)); // Adjust day if it exceeds max
                } else if (editingField == 5) {  // Year
                    newDate.Year = (newDate.Year + increment + 100) % 100;
                } else if (editingField == 6) {  // Format
                    timeFormat = !timeFormat;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for debouncing
    }
}



// Helper Function to Get Max Days in Month
int getMaxDaysInMonth(int month, int year) {
    switch (month) {
        case 4: case 6: case 9: case 11: return 30;
        case 2: return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
        default: return 31;
    }
}


// Display Chronograph Function
void displayChrono(void) {
    static bool displayInitialized = false;  // Display header once
    static uint32_t splitTimes[10];  // Array to store up to 10 split times
    static int splitCount = 0;       // Number of saved splits
    static int currentSplitIndex = -1;  // Index for viewing splits

    // Display header and controls once
    if (!displayInitialized) {
        LCD_Set_Cursor(0, 0);
        LCD_Send_String("    CHRONOGRAPH     ");
        LCD_Set_Cursor(2, 0);
        LCD_Send_String("[A]   [C] |         ");
        LCD_Set_Cursor(2, 3);
        LCD_Send_Data(LCD_CHAR_PLAY);  // Display the play icon
        LCD_Set_Cursor(2, 4);
        LCD_Send_Data(LCD_CHAR_PAUSE);  // Display the pause icon
        LCD_Set_Cursor(2, 9);
        LCD_Send_Data(LCD_CHAR_FLAG);  // Display the flag icon
        LCD_Set_Cursor(3, 0);
        LCD_Send_String("[B]   [D] |         ");
        LCD_Set_Cursor(3, 3);
        LCD_Send_Data(LCD_CHAR_MODE);
        LCD_Set_Cursor(3, 9);
        LCD_Send_Data(LCD_CHAR_RESET);
        displayInitialized = true;
    }

    // Calculate and display chrono time
    uint32_t chronoTime = chronoRunning ? (HAL_GetTick() - startTime) : elapsedTime;
    uint32_t minutes = (chronoTime / 60000) % 60;
    uint32_t seconds = (chronoTime / 1000) % 60;
    uint32_t tenths = (chronoTime / 100) % 10;
    sprintf(buffer, " %02lu:%02lu.%01lu  |", minutes, seconds, tenths);
    LCD_Set_Cursor(1, 0);
    LCD_Send_String(buffer);

    // Display split times in (1,11), (2,11), and (3,11)
    for (int i = 0; i < 3; i++) {
        int splitIndex = currentSplitIndex - (2 - i);
        if (splitIndex >= 0 && splitIndex < splitCount) {
            uint32_t split = splitTimes[splitIndex];
            uint32_t splitMin = (split / 60000) % 60;
            uint32_t splitSec = (split / 1000) % 60;
            uint32_t splitTenth = (split / 100) % 10;
            sprintf(buffer, "%02lu:%02lu.%01lu", splitMin, splitSec, splitTenth);
            LCD_Set_Cursor(1 + i, 11);
            LCD_Send_String(buffer);
        } else {
            LCD_Set_Cursor(1 + i, 11);
            LCD_Send_String("--:--.-");  // Clear split display if no split selected
        }
    }

    // Handle keypad input
    char key;
    if (xQueueReceive(xQueue, &key, 0)) {
        if (key == 'D') {  // Reset chronograph
            elapsedTime = 0;
            splitCount = 0;
            currentSplitIndex = -1;
            LCD_Clear();
            displayInitialized = false;
        } else if (key == '#') {  // Scroll down through splits
            if (splitCount > 0) {
                currentSplitIndex = (currentSplitIndex + 1) % splitCount;
            }
        } else if (key == '*') {  // Scroll up through splits
            if (splitCount > 0) {
                currentSplitIndex = (currentSplitIndex - 1 + splitCount) % splitCount;
            }
        } else if (key == 'C') {  // Split key
            if (splitCount < 10 && chronoRunning) {
                splitTimes[splitCount++] = chronoTime;
                currentSplitIndex = splitCount - 1;
            }
        } else if (key == 'A') {  // Start/Stop chronograph
            chronoRunning = !chronoRunning;
            if (chronoRunning) {
                startTime = HAL_GetTick() - elapsedTime;
            }
        } else if (key == 'B') {  // Return to TOD mode
            LCD_Clear();
            currentMode = TOD;
            isTodInitialized = false;
            displayInitialized = false;
        }
    }

    // Small delay for smooth transitions
    vTaskDelay(pdMS_TO_TICKS(10));
}


// Function to calculate the day of the week
int calculate_weekday(int day, int month, int year) {
	// If the month is January or February, adjust the month and year
    if (month < 3) {
        month += 12;
        year -= 1;
    }
    // Zeller's Congruence formula
    int K = year % 100; // The year of the century
    int J = year / 100; // The zero-based century (actually floor(year/100))
    int weekday = (day + (13 * (month + 1)) / 5 + K + (K / 4) + (J / 4) + (5 * J)) % 7;

    // Convert to 0=Saturday, 1=Sunday, ..., 6=Friday to 0=Sunday, 1=Monday, ..., 6=Saturday
    return (weekday + 6) % 7; // Adjusting to make Sunday = 0
}



/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
