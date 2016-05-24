/**
******************************************************************************
* @file    Button.h 
* @author  Logunov Viacheslav
* @date    
* @brief   
******************************************************************************
*/
#ifndef  __Button_H
#define  __Button_H


#ifdef __cplusplus
 extern "C" {
#endif

// раскоментировать если 
//	опрашивать кнопку по прерыванию
	
//#define BUTTON_CHECK_ON_INTRRPT

//отладочный пин
#define DEBUG_PIN // PD3
	
#ifdef DEBUG_PIN  
#define SET_DEB_PIN() GPIOD->BSRRL = GPIO_Pin_3
#define RST_DEB_PIN() GPIOD->BSRRH = GPIO_Pin_3
#else
#define SET_DEB_PIN() ((void)0)
#define RST_DEB_PIN() ((void)0)
#endif
	
	
#include "stm32f4xx.h"   
typedef enum 
{
  LdOff = 0,
  LdOn = 1,
  LdTogl = 2
} Led_Act; 

extern void vBut_BlueButtIni(void);
//extern void vBut_LEDOnOffTogl(char tClr, Led_Act En);
extern void vBut_Discovery_LEDInit();
//extern void vBut_ShowLeds(u16 DistSm  );
#ifdef BUTTON_CHECK_ON_INTRRPT
extern void vBut_IRQ_cBck(void);
#else
extern void vButtChckTask(void *pvParameters);
#endif
extern void vShowLedsTask(void *pvParameters);
extern u8 ucStartStop;



#ifdef __cplusplus
}
#endif

#endif /*__Button_H */
///////////////////////////////////////////////////


