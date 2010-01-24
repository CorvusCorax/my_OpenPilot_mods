/**
 ******************************************************************************
 *
 * @file       pios.c 
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2009.   
 * @brief      Sets up main tasks, tickhook, and contains the Main function.
 *                 - It all starts from here!
 * @see        The GNU Public License (GPL) Version 3
 * 
 *****************************************************************************/
/* 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


/* Project Includes */
#include "pios.h"

/* OpenPilot Includes */
#include <openpilot.h>

/* Task Priorities */

/* Local Variables */
static uint32_t ulIdleCycleCount = 0;
static uint32_t IdleTimePercent = 0;

/* Global Functions */
void vApplicationIdleHook(void);

/* Function Prototypes */
void TickTask(void *pvParameters);
void Flashy(void);
void SysTick_Handler(void);

/**
* Main function
*/
int main()
{
	
	/* Brings up System using CMSIS functions, enables the LEDs. */
	PIOS_SYS_Init();

	/* Delay system */
	PIOS_DELAY_Init();

	/* SPI Init */
	PIOS_SPI_Init();

	/* Enables the SDCard */
	PIOS_SDCARD_Init();

	/* Wait for SD card for ever */
	for(;;)
	{
		/* Check if we have an SD Card */
		if(!PIOS_SDCARD_MountFS(STARTUP_LOG_ENABLED)) {
			/* Found one without errors */
			break;
		}

		/* SD Card not found, flash for 1 second */
		PIOS_LED_On(LED1);
		PIOS_LED_On(LED2);
		for(uint32_t i = 0; i < 10; i++) {
			PIOS_LED_Toggle(LED2);
			PIOS_DELAY_Wait_mS(100);
		}
	}

	/* Call LoadSettings which populates global variables so the rest of the hardware can be configured. */
	PIOS_Settings_Load();

	/* Com ports init */
	PIOS_COM_Init();

	/* Initialise servo outputs */
	PIOS_Servo_Init();

	/* Used to test servos, cycles all servos from one side to the other
	for(;;) {
		for(int i = 1000; i < 2000; i++) {
			PIOS_Servo_Set(0, i);
			PIOS_Servo_Set(1, i);
			PIOS_Servo_Set(2, i);
			PIOS_Servo_Set(3, i);
			PIOS_Servo_Set(4, i);
			PIOS_Servo_Set(5, i);
			PIOS_Servo_Set(6, i);
			PIOS_Servo_Set(7, i);
			PIOS_DELAY_Wait_uS(500);
		}
		for(int i = 2000; i > 1000; i--) {
			PIOS_Servo_Set(0, i);
			PIOS_Servo_Set(1, i);
			PIOS_Servo_Set(2, i);
			PIOS_Servo_Set(3, i);
			PIOS_Servo_Set(4, i);
			PIOS_Servo_Set(5, i);
			PIOS_Servo_Set(6, i);
			PIOS_Servo_Set(7, i);
			PIOS_DELAY_Wait_uS(500);
		}
	}
	*/

	Flashy();
	
	/* Analog to digi init */
//	PIOS_ADC_Init();
	
	/* Initialise OpenPilot application */
//	OpenPilotInit();

	/* Create a FreeRTOS task */
	xTaskCreate(TickTask, (signed portCHAR *) "Test", configMINIMAL_STACK_SIZE , NULL, 2, NULL);

	/* Start the FreeRTOS scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be running. */
	/* If we do get here, it will most likely be because we ran out of heap space. */
	return 0;
}

void Flashy(void)
{
	for(;;)
	{
		/* Setup the LEDs to Alternate */
		PIOS_LED_On(LED1);
		PIOS_LED_Off(LED2);

		/* Infinite loop */
		for(;;)
		{
			PIOS_LED_Toggle(LED1);
			//PIOS_LED_Toggle(LED2);
			PIOS_DELAY_Wait_mS(250);
		}
	}
}

void TickTask(void *pvParameters)
{
	const portTickType xDelay = 10 / portTICK_RATE_MS;

	for(;;)
	{
		PIOS_LED_Toggle(LED2);
		vTaskDelay(xDelay);
	}
}


/**
* Idle hook function
*/
void vApplicationIdleHook(void)
{
	/* Called when the scheduler has no tasks to run */

	/* In here we could implement full stats for FreeRTOS
	Although this would need us to enable stats in FreeRTOS
	which is *very* costly. With the function below we can
	either print it out or just watch the variable using JTAG */
	
	/* This can give a basic indication of how much time the system spends in idle */
	/* IdleTimePercent is the percentage of time spent in idle since the scheduler started */
	/* For example a value of 75 would mean we are spending 75% of FreeRTOS Cycles in idle */
	ulIdleCycleCount++;
	IdleTimePercent = ((ulIdleCycleCount / xTaskGetTickCount()) * 100);
}

