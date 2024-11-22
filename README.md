# FreeRTOS Digital Watch

A **Digital Watch** project implemented using **FreeRTOS** on an STM32F407 microcontroller. This project demonstrates efficient multitasking for real-time applications, featuring a user-friendly interface and multiple modes.

---

## Features
- **Time of Day (TOD):** Displays the current time and date with options to adjust settings.
- **Chronograph Mode:**
  - Start/Stop functionality.
  - Split times with scrolling.
  - Reset functionality.
- **Timer Mode:** Countdown timer with user-defined settings.
- **User Input:** Responsive 4x4 keypad for switching modes and performing actions.
- **LCD Display:** 20x4 LCD for clear and dynamic display of information.

---

## Hardware Specifications
| **Component**        | **Description**                                                                 |
|-----------------------|---------------------------------------------------------------------------------|
| **STM32F407 MCU**     | Cortex-M4-based microcontroller with embedded RTC and timers.                  |
| **20x4 LCD Display**  | Displays the time, date, chronograph, and timer details.                       |
| **4x4 Keypad**        | Used for user input to navigate and interact with the watch's modes.           |
| **RTC Module**        | Ensures accurate timekeeping for TOD and Timer functionalities.                |
| **Resistors/Capacitors** | Supporting circuitry for stable operation of the MCU and peripherals.       |

---

## Software Specifications
| **Specification**        | **Description**                                                                 |
|---------------------------|---------------------------------------------------------------------------------|
| **FreeRTOS Version**      | FreeRTOS kernel used for task management and real-time performance.             |
| **Task Management**       | Dedicated tasks for TOD, Chronograph, Timer, and Keypad Input.                  |
| **LCD Update**            | Efficient refresh only for parts of the LCD that change.                       |
| **RTC Integration**       | Uses STM32 RTC peripheral for precise time tracking.                           |
| **Keypad Debouncing**     | Software debouncing for accurate user input.                                   |
| **Chronograph Storage**   | Stores up to 10 split times with scrolling functionality.                      |
| **Peripheral Drivers**    | HAL drivers used for hardware abstraction and cleaner code implementation.     |

---

## Project Architecture
```plaintext
FreeRTOS-Digital-Watch/
├── Core/
│   ├── Inc/               # Header files
│   ├── Src/               # Source files
│   └── FreeRTOSConfig.h   # FreeRTOS configuration
├── Drivers/
│   ├── CMSIS/             # ARM Cortex-M CMSIS files
│   ├── STM32F4xx_HAL/     # STM32 HAL drivers
├── ThirdParty/
│   ├── FreeRTOS/          # FreeRTOS kernel files
├── Docs/                  # Documentation (Hardware/Software specifications)
├── Scripts/               # Helper scripts (e.g., build scripts)
├── LICENSE                # License file
├── README.md              # Project overview
└── FreeRTOS-Digital-Watch.ioc  # STM32CubeMX project file
