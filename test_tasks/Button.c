/**
******************************************************************************
* @file    Button.c 
* @author  Logunov Viacheslav
* @date    __DATE__
* @brief  опрос состояния кнопки и работа св диодов
******************************************************************************
*/
#include "Button.h" 
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "my_HCSR.h" 


/* Private variables ---------------------------------------------------------*/
u8 ucStartStop;

#ifdef BUTTON_CHECK_ON_INTRRPT

/**
  * @brief Exti0 IRQ CallBack 
*/
void vBut_IRQ_cBck(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    ucStartStop ^=1;
    
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  } 
}
#else
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                               vButtChckTask()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void vButtChckTask(void *pvParameters)
{
  static u16 Cnt; 
  static u16 Iter;
  while(1)
  {
    taskENTER_CRITICAL(); 
    Iter = 1000;
    Cnt =0;
    while(Iter--)
    {
      if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) != 0)
      Cnt++;  
    }
    if(Cnt > 700)
    {
      while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) != 0){};
      ucStartStop ^=1;
    }
    taskEXIT_CRITICAL();
  } ;
  vTaskDelete(NULL);	 
}
#endif

/**
  *  Инициализация кнопки
  */
void vBut_BlueButtIni(void)
{
  
  GPIO_InitTypeDef   GPIO_InitStructure;
  

  /* Enable GPIOA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Configure PA0 pin as input pullup */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
#ifdef BUTTON_CHECK_ON_INTRRPT
  NVIC_InitTypeDef   NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
    /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Connect EXTI Line0 to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  
  /* Configure EXTI Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif 
  ucStartStop = 1;
}


//=======================LEDs================


/**
  * @brief  Инициализация светодиодов и debug пина
  */
void vBut_Discovery_LEDInit()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|
                                GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
#ifdef DEBUG_PIN  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif
}



////////////////////////////////////////////////////
//           ShowLeds()  
///////////////////////////////////////////////////
void vBut_ShowLeds(u16 DistSm  )
{
  static u8 Meter;
  static u8 bit_toshow;
  if(DistSm !=0)
  {
    
    Meter  = DistSm/50;
    bit_toshow = 0;
    switch (Meter)
    {
    case 0: bit_toshow = 0;  break;  
    case 1: bit_toshow = 1;  break;
    case 2: bit_toshow = 3;  break;
    case 3: bit_toshow = 7;  break;
    default: bit_toshow = 0xF;  break;
    }
	portDISABLE_INTERRUPTS(); // чтоб не мерцали
    GPIOD->BSRRH = (0xF<<12);// гасим все
    GPIOD->BSRRL = (bit_toshow<<12);
	portENABLE_INTERRUPTS();
  }
  
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                               vShowLedsTask()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void vShowLedsTask(void *pvParameters)
{
  while(1)
	 {
	   vBut_ShowLeds(usCurrentDistance);
	 }
  vTaskDelete(NULL);	 
}
//*****************************************************************************

//
//const char LED_COLR[6] = "GORB\0";
///*
//светодиоды STM32F4Discovery платы
//выкл вкл или инвертировать св диод
//  @param tClr - 'R' 'G' 'B' 'O'
//         красн, зел, син, оранж
//
//  @param En 0, 1, 2
//            Off, On, Toggl
//
//*/
//void vBut_LEDOnOffTogl(char tClr, Led_Act En)
//{
//  char* a = strchr(LED_COLR,tClr);
//  if(a != 0)
//  {
//    int d = (u32)(a - LED_COLR) + 12;
//    uint16_t tPin = (1<<d);
//    if(En == LdOn)
//    {
//      GPIOD->BSRRL = tPin;
//    }
//    else if(En == LdOff)
//    {
//      GPIOD->BSRRH = tPin;
//    }
//    else if(En == LdTogl)
//    {
//      GPIOD->ODR ^= tPin;
//    }
//  }
//  
//}	
///////////////////////////////////////////////////


