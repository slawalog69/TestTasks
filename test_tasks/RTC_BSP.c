/**
******************************************************************************
* @file    RTC_BSP.c 
* @author  Logunov Viacheslav
* @date    
* @brief  работа с таймером реального времени 
******************************************************************************
*/
#include "RTC_BSP.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "Button.h" 


/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef RTC_InitStructure;
//глобальные
RTC_TimeTypeDef RTC_TimeStructure;
RTC_DateTypeDef RTC_DateStructre;

/**
* @brief  Получение текущих секунд
*/
uint32_t ulRTC_Bsp_GetNow(void)
{
  uint32_t t_reg = (uint32_t)(RTC->TR & RTC_TR_MASK);
  t_reg &=  (RTC_TR_SU |RTC_TR_ST); // 
  return t_reg;
}
/**
* @brief  Configures the RTC.
*/
void vRTC_Bsp_Config(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
  
  {
    /* RTC Configuration ******************************************************/
    /* Reset Backup Domain */
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);
    
    /* Запуск внутреннего осц 32кГц(LSI) */ 
    RCC_LSICmd(ENABLE);
    
    /* Подождать внутренний осц запустится */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET){};
    
    /* Выбор тактирования RTC от LSI */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    /* Запуск RTC  */
    RCC_RTCCLKCmd(ENABLE);
    
    /* Ждем пока регистры даты и времени синхронизируются с APB*/
    RTC_WaitForSynchro();
    
    /* Устанавливаем тик RTC как 1с (1/32768 если принять, что LSI - точно 32768Hz,
    иначе - нужно клибровать)*/
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0x00FF;
    
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      while(1);
    }
    RTC_TimeStructInit(&RTC_TimeStructure);
    /* уст время  23h 59mn 50s */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x23;
    RTC_TimeStructure.RTC_Minutes = 0x59;
    RTC_TimeStructure.RTC_Seconds = 0x50;  
    
    if(RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure) == ERROR)
    {
      while(1);
    }
    /* уст дату  пятница 31 декабря 15 года - типа путин всех поздравляет*/
    RTC_DateStructInit(&RTC_DateStructre);
    
    RTC_DateStructre.RTC_WeekDay =  RTC_Weekday_Friday;
    RTC_DateStructre.RTC_Month = RTC_Month_December;
    RTC_DateStructre.RTC_Year = 0x15;
    RTC_DateStructre.RTC_Date = 0x31;
    
    if(RTC_SetDate(RTC_Format_BCD, &RTC_DateStructre) == ERROR)
    {
      while(1);
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                               vRTCTask()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void vRTCTask(void *pvParameters)
  {
	 while(1)
	   {
		  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructre);
		  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	   }
	 vTaskDelete(NULL);	
  }
//*****************************************************************************



