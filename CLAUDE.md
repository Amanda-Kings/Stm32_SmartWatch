# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

This is a bare-metal STM32F103C8 smartwatch firmware project built with Keil MDK/uVision 5 using the ARM Compiler 5 toolchain. The project uses the STM32F10x Standard Peripheral Library plus local BSP drivers and a small page-based UI framework.

There is no CMake/Makefile or host-side unit test runner in the repository. The authoritative build configuration is `Project/project.uvprojx`.

## Build and development commands

- Build the firmware in Keil uVision: open `Project/project.uvprojx`, select target `Target 1`, then build. Output is configured under `Project/Objects/`, and hex generation is enabled as `Project/Objects/project.hex`.
- The configured target is `STM32F103C8`, ARM Compiler `V5.06 update 7 (build 960)`, using package `Keil.STM32F1xx_DFP.2.4.1`.
- If Keil's command-line executable is available on PATH, build with:
  ```sh
  UV4 -b Project/project.uvprojx -t "Target 1"
  ```
  Otherwise use the full local Keil path for `UV4.exe`.
- Clean/rebuild is normally done in uVision. From CLI, use:
  ```sh
  UV4 -r Project/project.uvprojx -t "Target 1"
  ```
- There is no lint command. VS Code IntelliSense is configured in `.vscode/` for C99-style checking with defines `USE_STDPERIPH_DRIVER`, `STM32F10X_MD`, and `__GNUC__`.
- There is no automated single-test command. Test code lives in `Test/Src/test.c`; enable the test entry path by defining `TEST_DeBUG` in `Src/main.c`, then rebuild and flash/run on hardware.

## Architecture

### Startup and main loop

`Src/main.c` is the firmware entry point. In normal mode it calls `BSP_Init()`, then `App_Init()`, then repeatedly calls `App_Task()`.

`Src/bsp_init.c` initializes board peripherals in this order: DWT delay support, RTC, UART1, OLED, SysTick, and keys. SysTick is configured as a 1 ms system tick in `BSP/MCU/Src/bsp_sys_tick.c`, and `SysTick_Handler()` in `Src/stm32f10x_it.c` increments the tick.

### Layering

- `Lib/` contains CMSIS and STM32F10x Standard Peripheral Library sources.
- `BSP/MCU/` contains MCU-level drivers and services such as UART, software/hardware I2C, RTC, SysTick, and DWT timing.
- `BSP/Dev/` contains board/device drivers such as OLED, OLED font/image data, LED, and keys.
- `Utils/` contains utility code such as blocking delays.
- `App/Core/` owns application initialization and the periodic app task.
- `App/UI/Core/` implements the UI page registry, page switching, key dispatch, and timed refresh.
- `App/UI/Page/` contains concrete pages such as home and settings.
- `Test/` contains hardware-oriented test routines selected manually from `main.c`.

### UI framework

The UI is built around `UI_Page` objects defined in `App/UI/Core/Inc/ui_core.h`. Each page registers itself with `UI_RegisterPage()` and provides a vtable with `on_enter`, `draw`, and `on_key_event` callbacks.

`App/Core/Src/app.c` wires key callbacks for `KEY_UP`, `KEY_DOWN`, and `KEY_ENTER` to `UI_HandleKeyEvent()`, registers pages, configures page refresh intervals, and selects the home page as the default.

`UI_Refresh()` redraws only the current page and only when its `refresh_interval` is nonzero and elapsed. Pages with interval `0` draw on entry and explicit key actions instead of periodic refresh.

Adding a page generally requires:
1. Add a new ID to `UI_Page_TypeDef` before `UI_PAGE_MAX`.
2. Create the page implementation under `App/UI/Page/...` with a static `UI_Page`, static menu item table if needed, and a `Page_X_Init()` function that calls `UI_RegisterPage()`.
3. Call `Page_X_Init()` and `SetPageRefreshInterval()` from `App_Init()`.
4. Add the new source file and include path to `Project/project.uvprojx` so Keil builds it.

### Input model

`BSP/Dev/Src/bsp_key.c` implements a polling key state machine for `KEY_UP`, `KEY_DOWN`, and `KEY_ENTER`. `App_Task()` scans keys every 10 ms, so debounce/long/repeat constants are expressed in scan counts: 2 = 20 ms debounce, 50 = 500 ms long press, 20 = 200 ms repeat.

Key events are delivered through registered callbacks as `KEY_EVENT_SHORT`, `KEY_EVENT_LONG`, `KEY_EVENT_REPEAT`, and `KEY_EVENT_RELEASE`. Current UI pages typically react to short presses for navigation; the date/time setting page also uses repeat events while editing fields.

### Display model

`BSP/Dev/Src/bsp_oled.c` maintains a 128x64 monochrome shadow buffer `OLED_DisplayBuf[8][128]`. Drawing APIs update the buffer; the display changes only after calling `OLED_Update()` or `OLED_UpdateArea()`.

The OLED uses software I2C and address `0x78`. UI pages usually call `OLED_Clear()`, render strings/images/shapes into the buffer, optionally call `OLED_ReverseArea()` for focus highlighting, then call `OLED_Update()`.

### RTC/date-time flow

`BSP/MCU/Src/bsp_rtc.c` initializes the STM32 backup-domain RTC from LSE. On first boot, identified by backup register `BKP_DR1 != 0xA5A5`, it sets the initial timestamp from constants in `BSP/MCU/Inc/bsp_rtc.h`; later boots keep the existing RTC counter.

The home page reads `RTC_GetTimeStamp()` for display. The date/time settings page edits a local `DateTime` copy and writes it back with `RTC_SetTimeStamp()` when ENTER confirms a field.

## Keil project maintenance

`Project/project.uvprojx` must be kept in sync with source changes. Adding a `.c` file to the repository is not enough; add it to the appropriate uVision group so it participates in the firmware build. Include paths are also maintained in the Keil project file.

The project currently builds with generated artifacts under `Project/Objects/` and `Project/Listings/`. Treat those as build outputs unless specifically inspecting build logs.
