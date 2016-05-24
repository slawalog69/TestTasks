/**
******************************************************************************
* @file    Irq.h 
* @author  Logunov Viacheslav
* @date    
* @brief   
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IRQ_H
#define __IRQ_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
//void SysTick_Handler(void);
void EXTI0_IRQHandler(void);

#endif /* __IRQ_H */


