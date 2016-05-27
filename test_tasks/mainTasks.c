/**
******************************************************************************
* @file    mainTasks.c 
* @author  Logunov Viacheslav
* @date    
* @brief  Èíèöèàëèçàöèÿ è îñíîâíàÿ task 
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "mainTasks.h"
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h> 
#include "ADC_BSP.h" 
#include "RTC_BSP.h" 
#include "USART_BSP.h"
#include "Button.h"
#include "my_HCSR.h" 


const char weekDays[7][4] = {"MON\0","TUE\0","WED\0","THU\0","FRI\0","SAT\0","SUN\0"};

/*******************************************************************/
void vInitAll()
{
  /* RTC configuration */
  vRTC_Bsp_Config();

  /* ADC configuration */
  vAdc_Ini();
  /* USART2 configuration */
  vUSART2_ini();
  /* Ini User Button  */
  vBut_BlueButtIni();
  /* Ini Leds of STMF4Discovery */
  vBut_Discovery_LEDInit();
  /* Ini äàò÷èê HCSR-04  */
  vHCSR_Ini();
  
}

/*******************************************************************/
// âûâîä ðåçóëüòàòîâ â PC
void vMainTask (void *pvParameters)
{
  static uint8_t CurrSecs =100;
  static uint32_t t_Now;  
  while (1)
  {
    t_Now = ulRTC_Bsp_GetNow();
    if(CurrSecs != (uint8_t)t_Now)		//êàæäóþ ñåêóíäó
    {
      if((ucGotAverage ==1)&&(ucStartStop == 1))
      {
        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStructre);
        
        
        sprintf(cTxBuff, "{\"Date\":\"%s %02d.%02d.%02d %02d:%02d:%02d\",\"Temperature\":\"%0.1f\",\"Dist\":%d}\n", 
                weekDays[RTC_DateStructre.RTC_WeekDay -1],
                RTC_DateStructre.RTC_Year,
                RTC_DateStructre.RTC_Month,
                RTC_DateStructre.RTC_Date,
                RTC_TimeStructure.RTC_Hours,
                RTC_TimeStructure.RTC_Minutes,
                RTC_TimeStructure.RTC_Seconds,
                dCurrentTermo,
				usCurrentDistance);
        
        vUSART2_send(cTxBuff, strlen(cTxBuff)); 
      }
      CurrSecs = (uint8_t)t_Now; 
    }
  }
  vTaskDelete(NULL);	
}

//----------END OF FILE---------------

