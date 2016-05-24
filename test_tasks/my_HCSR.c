/**
******************************************************************************
* @file    my_HCSR.c 
* @author  Logunov Viacheslav
* @date    __DATE__
* @brief   работа с датчиком HC SR-04
******************************************************************************
*/
#include "my_HCSR.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "Button.h" 

/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_BaseStruct;
u32 curUs;
//глобальные
u16 usCurrentDistance;
////////////////////////////////////////////////////
//           Delay()  
///////////////////////////////////////////////////
void vHCSR_Delay( u32 uS )
{
   TIM_Cmd(TIM4,DISABLE);
   TIM_ClearFlag( TIM4, TIM_FLAG_CC1);
   TIM4->CCR1 = uS;
   TIM4->CNT = 0;
   TIM_Cmd(TIM4,ENABLE);
   while((TIM4->SR & TIM_FLAG_CC1) == 0){};
   TIM_Cmd(TIM4,DISABLE);
}
////////////////////////////////////////////////////
//           StartTim()  
///////////////////////////////////////////////////
void vHCSR_StartTim(  )
{
  TIM_Cmd(TIM4,DISABLE);
  TIM4->CNT = 0;
  
  // устанавливаем период между импульсами
  //TIM_ClearFlag( TIM4, TIM_FLAG_CC2);
  TIM4->CCR2 = 50000;
  //------------------------
  TIM_ClearFlag( TIM4, 0x1F);
  TIM_Cmd(TIM4,ENABLE); 
}
////////////////////////////////////////////////////
//           Rst_Trigg()  
///////////////////////////////////////////////////
void vHCSR_Rst_Trigg(  )
{
  TRIGG_GPIO_PORT ->BSRRH = TRIGG_PIN;
}
////////////////////////////////////////////////////
//           Set_Trigg()  
///////////////////////////////////////////////////
void vHCSR_Set_Trigg(  )
{
  TRIGG_GPIO_PORT ->BSRRL = TRIGG_PIN;
}


////////////////////////////////////////////////////
//           Get_Time_Echo() 
// Выбран примитивный метод подсчета времени поскольку
// в ТЗ поставлена дискретность определения расстояния - 0.5м
///////////////////////////////////////////////////
u16 usHCSR_Get_Distance(  )
{
 // taskDISABLE_INTERRUPTS(); 
  vHCSR_Rst_Trigg();
  vHCSR_Delay(2);
  vHCSR_Set_Trigg();
  /* Delay 10 us */
  vHCSR_Delay(10);
  vHCSR_Rst_Trigg();
  
 // ждать пока echo пин = 0
  vHCSR_StartTim();
  while((ECHO_GPIO_PORT->IDR & ECHO_PIN) == 0)
  {
    // если таймер переполнился (~65ms)
	 if((TIM4->SR & TIM_FLAG_CC2) != 0){
       return 0;
	 }
  };

  vHCSR_StartTim();
  // ждать пока echo пин = 1
  while((ECHO_GPIO_PORT->IDR & ECHO_PIN) != 0)
  {
    // если таймер переполнился (~65ms)
	 if((TIM4->SR & TIM_FLAG_CC2) != 0){
       return 0;
	 }
  };
  
  curUs = TIM4->CNT; 
  /* Convert us to cm */
  u16 Distance =  curUs /58;
  
  //ждем гарантированного времени между импульсами
  while((TIM4->SR & TIM_FLAG_CC2) == 0){};
  //taskENABLE_INTERRUPTS();
 
  return Distance;
}
////////////////////////////////////////////////////
//           TIM_Config()   
///////////////////////////////////////////////////

void vHCSR_TIM_Config(void)
{
  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_BaseStruct);
  TIM_BaseStruct.TIM_Period = 0x0000FFFF;
  TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
  
  
  RCC_ClocksTypeDef ClckStrct;
  RCC_GetClocksFreq(&ClckStrct);
  
  // Настройка на 1us
  u32 PrescalerValue = (uint16_t) ((ClckStrct.PCLK1_Frequency * 2) / 1000000) - 1;
  
  TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);

}
////////////////////////////////////////////////////
//           Ini_HCSR_04()   
///////////////////////////////////////////////////
void vHCSR_Ini(  )
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* clock enable */
  RCC_AHB1PeriphClockCmd(ECHO_GPIO_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(TRIGG_GPIO_CLK, ENABLE);
  
  /* конфигурация эхо пина от HCSR_04 */
  GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(ECHO_GPIO_PORT, &GPIO_InitStructure);
  
  /* конфигурация тригг пина к HCSR_04 */
  GPIO_InitStructure.GPIO_Pin = TRIGG_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(TRIGG_GPIO_PORT, &GPIO_InitStructure); 
  
  vHCSR_TIM_Config(); 
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                               vHCSR_Task()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void vHCSR_Task(void *pvParameters)
{
    // счетчик 
  portTickType xLastWakeTime;
  const portTickType xBetween = 250;
  // Инициализируем xLastWakeTime текущим временем
  xLastWakeTime = xTaskGetTickCount();
   
   
  while(1)
	 {
		taskENTER_CRITICAL();
		SET_DEB_PIN();
		usCurrentDistance =  usHCSR_Get_Distance();
		RST_DEB_PIN();
		taskEXIT_CRITICAL();
		vTaskDelayUntil(&xLastWakeTime, xBetween);
		
	 }
  vTaskDelete(NULL);	 
}

///////////////////////////////////////////////////

