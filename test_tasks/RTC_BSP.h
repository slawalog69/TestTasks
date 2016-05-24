/**
******************************************************************************
* @file    RTC_BSP.h 
* @author  Logunov Viacheslav
* @date    
* @brief   
******************************************************************************
*/
#ifndef  __RTC_BSP_H
#define  __RTC_BSP_H


#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
   
#define RTC_TR_MASK    ((uint32_t)0x007F7F7F)
extern RTC_TimeTypeDef RTC_TimeStructure;
extern RTC_DateTypeDef RTC_DateStructre;
extern void vRTC_Bsp_Config(void);

extern uint32_t ulRTC_Bsp_GetNow(void);
extern void vRTCTask(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /*__RTC_BSP_H */
///////////////////////////////////////////////////


