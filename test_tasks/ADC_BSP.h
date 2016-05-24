/**
******************************************************************************
* @file    ADC_BSP.h 
* @author  Logunov Viacheslav
* @date    
* @brief   
******************************************************************************
*/
#ifndef  __ADC_BSP_H
#define  __ADC_BSP_H


#ifdef __cplusplus
 extern "C" {
#endif
//#include "stm32f4_discovery.h"
 #include "stm32f4xx.h"
extern u8 ucGotAverage;
extern double dCurrentTermo; 	
extern void vAdc_Ini() ;
//extern u16 usAdc_Temperature_read();
//extern double dAdc_CalculateThermo(u16 ADC_Val);

extern void vAdcTask(void *pvParameters );

#ifdef __cplusplus
}
#endif

#endif /*__ADC_BSP_H */
///////////////////////////////////////////////////


