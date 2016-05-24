/**
******************************************************************************
* @file    USART_BSP.h 
* @author  Logunov Viacheslav
* @date    
* @brief   
******************************************************************************
*/
#ifndef  __USART_BSP_H
#define  __USART_BSP_H


#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
	
#define USARTx                   			(USART2) 
#define UART_CLK                    		RCC_APB1Periph_USART2  
#define UART_IRQ                    		USART2_IRQn 
	

	
#define TX_UART_PIN                         GPIO_Pin_2
#define TX_UART_GPIO_PORT                   GPIOA
#define TX_UART_GPIO_CLK                    RCC_AHB1Periph_GPIOA  
#define TX_UART_PinSource                   GPIO_PinSource2  
#define RX_UART_PIN                         GPIO_Pin_3
#define RX_UART_GPIO_PORT                   GPIOA
#define RX_UART_GPIO_CLK                    RCC_AHB1Periph_GPIOA  
#define RX_UART_PinSource                   GPIO_PinSource3  
#define UART_AF_GPIO                  		GPIO_AF_USART2  

extern void vUSART2_ini();
extern void vUSART2_send(void *Buff, int len);
extern u8 ucGotMess;
extern char cTxBuff[200];
extern void vUSART_CBackHndl();
#ifdef __cplusplus
}
#endif

#endif /*__USART_BSP_H */
///////////////////////////////////////////////////


