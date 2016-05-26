/**
******************************************************************************
* @file    my_HCSR.h 
* @author  Logunov Viacheslav
* @date    
* @brief   
******************************************************************************
*/
#ifndef  __my_HCSR_H
#define  __my_HCSR_H


#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

  
#define TRIGG_PIN                         GPIO_Pin_6
#define TRIGG_GPIO_PORT                   GPIOB
#define TRIGG_GPIO_CLK                    RCC_AHB1Periph_GPIOB  
#define ECHO_PIN                         GPIO_Pin_7
#define ECHO_GPIO_PORT                   GPIOB
#define ECHO_GPIO_CLK                    RCC_AHB1Periph_GPIOB  
  
extern void vHCSR_Ini(void );
//extern u16 usHCSR_Get_Distance(  );
//extern u32 curUs;
extern u16 usCurrentDistance;
extern void vHCSR_Task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /*__my_HCSR_H */
///////////////////////////////////////////////////


