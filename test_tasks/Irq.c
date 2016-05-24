/**
******************************************************************************
* @file    Irq.c 
* @author  Logunov Viacheslav
* @date    
* @brief   переопределения обработчиков
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "Irq.h"
#include "stm32f4xx.h"
#include "USART_BSP.h" 
#include "Button.h" 

/* Private variables ---------------------------------------------------------*/
extern u32 Time_t;
/* Private function prototypes -----------------------------------------------*/

/******************************************************************************/
                                           

void USART2_IRQHandler(void)
{
  vUSART_CBackHndl();
  
}
//**********
void EXTI0_IRQHandler(void)
{
#ifdef BUTTON_CHECK_ON_INTRRPT
  vBut_IRQ_cBck();
#endif
}

