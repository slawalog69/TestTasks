/*
FreeRTOS V8.0.1 - Copyright (C) 2014 Real Time Engineers Ltd.
All rights reserved

*/


/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


/* Hardware and starter kit includes. */
//#include "arm_comm.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h> 
#include "ADC_BSP.h" 
#include "RTC_BSP.h" 
#include "USART_BSP.h"
#include "Button.h"
#include "my_HCSR.h" 
#include "mainTasks.h"


/*-----------------------------------------------------------*/

int main(void)
{
 
  vInitAll();
  xTaskCreate(vMainTask,(char*)"MainTask", configMINIMAL_STACK_SIZE+256, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(vAdcTask,(char*)"AdcTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(vHCSR_Task,(char*)"HCSR_Task", configMINIMAL_STACK_SIZE+256, NULL, tskIDLE_PRIORITY + 1, NULL);
#ifndef BUTTON_CHECK_ON_INTRRPT 
  xTaskCreate(vButtChckTask,(char*)"ButtChckTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
#endif
  xTaskCreate(vHCSR_Task,(char*)"HCSR_Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(vShowLedsTask,(char*)"ShowLedsTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  
  
  vTaskStartScheduler();
  
}


///*-----------------------------------------------------------*/
//
void vApplicationTickHook( void )
{

}

/*-----------------------------------------------------------*/
//
void vApplicationMallocFailedHook( void )
{

  for( ;; );
}
///*-----------------------------------------------------------*/
//
void vApplicationIdleHook( void )
{

}
///*-----------------------------------------------------------*/
//
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
  ( void ) pcTaskName;
  ( void ) pxTask;

  taskDISABLE_INTERRUPTS();
  for( ;; );
}

/*******************************************************************/

