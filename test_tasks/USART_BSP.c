/**
******************************************************************************
* @file    USART_BSP.c 
* @author  Logunov Viacheslav
* @date    __DATE__
* @brief  с
******************************************************************************
*/
#include "USART_BSP.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"


/* Private variables ---------------------------------------------------------*/
char * chToSend;
char * chEnd;

u8 ucGotMess =0;
u8 ucTX_isBusy =0;
struct _RecStrct
{
  u8 gotHead;
  char RecivBuf[200];
  char * chRec;
}xRecStrct;
char cTxBuff[200];

USART_InitTypeDef USART_IniStrct;
///////////////////////////////////////////////////


/********************uart************************************/
// Инициализация usart2
void vUSART2_ini()
{
  GPIO_InitTypeDef GPIO_IniStrct; 
  // Запускаем тактирование
  
  if ((USARTx == USART1)||(USARTx == USART6))
	 RCC_APB2PeriphClockCmd(UART_CLK, ENABLE);  
  else
	 RCC_APB1PeriphClockCmd(UART_CLK, ENABLE);  
  
  RCC_AHB1PeriphClockCmd(TX_UART_GPIO_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(RX_UART_GPIO_CLK, ENABLE);
  
  // Инициализация пинов  USART
  GPIO_StructInit(&GPIO_IniStrct);
  
  GPIO_IniStrct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_IniStrct.GPIO_Pin = TX_UART_PIN;
  GPIO_IniStrct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_IniStrct.GPIO_OType = GPIO_OType_PP;
  GPIO_IniStrct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(TX_UART_GPIO_PORT, &GPIO_IniStrct);
  
  GPIO_IniStrct.GPIO_Pin = RX_UART_PIN;
  GPIO_Init(RX_UART_GPIO_PORT, &GPIO_IniStrct);
  
  // Определяем альтернативную функцию вывода
  GPIO_PinAFConfig(TX_UART_GPIO_PORT, TX_UART_PinSource, UART_AF_GPIO);
  GPIO_PinAFConfig(RX_UART_GPIO_PORT, RX_UART_PinSource, UART_AF_GPIO);
  
  // настраиваем периферию USART
  USART_StructInit(&USART_IniStrct);
  USART_IniStrct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_IniStrct.USART_BaudRate = 115200;	
  USART_Init(USARTx, &USART_IniStrct);
  
  xRecStrct.gotHead =0;
  xRecStrct.chRec = xRecStrct.RecivBuf;
  
  // Включаем прерывания и запускаем USART
  
  USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
  
  NVIC_EnableIRQ(UART_IRQ);
  USART_Cmd(USARTx, ENABLE);
}



/**
* @brief  USARTx IRQ callback
* @param  None
* @retval None
*/
void vUSART_CBackHndl()
{
  char tRec;

  // прерывание - transmit complete
  if (USART_GetITStatus(USARTx, USART_IT_TC) != RESET)
  {
    // Очищаем флаг прерывания 
    USART_ClearITPendingBit(USARTx, USART_IT_TC);
    // если все отправили
    if(chToSend >= chEnd)
    {
      USART_ITConfig(USARTx, USART_IT_TC, DISABLE);
      ucTX_isBusy =0;
      return;
    }
    // Отправляем байт данных  
      USARTx->DR =  (*chToSend);
    chToSend++;   
  }
  //-----------
  // прерывание - rx не пустой
  if (USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)
  {
    // Очищаем флаг прерывания 
    USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
    //вычитываем
    tRec =USART_ReceiveData(USARTx);
    
    // если предыдущая команда не прочитана в основном цикле - пропадает следующая 
    if(ucGotMess) return;
    
    *xRecStrct.chRec = tRec; 
    // приняли заголовок команды - '@'
    if(xRecStrct.gotHead == 0)
    {
      if( *xRecStrct.chRec == '@')
        xRecStrct.gotHead =1;
      return;
    }
    if( *xRecStrct.chRec == '\n')
    {
      ucGotMess = 1; 
      xRecStrct.gotHead =0;
      xRecStrct.chRec[1] =0;
      xRecStrct.chRec = xRecStrct.RecivBuf;
      return;
    }
    xRecStrct.chRec++;
    
    
  }
} 

void vUSART2_send(void *Buff, int len)
{
  char * tBuf;
  char tSnd;
  ucTX_isBusy = 1;
  
  USART_ClearITPendingBit(USARTx, USART_IT_TC);
  
  tBuf = (char*)Buff;
  chToSend =&tBuf[1];
  chEnd =  &tBuf[len];
  tSnd = *tBuf;
  USARTx->DR = tSnd ;
  
  USART_ITConfig(USARTx, USART_IT_TC, ENABLE);
  while(ucTX_isBusy == 1){};
}


