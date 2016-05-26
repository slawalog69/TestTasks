/**
******************************************************************************
* @file    ADC_BSP.c 
* @author  Logunov Viacheslav
* @date    
* @brief  измерение температуры кристалла  
******************************************************************************
*/
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ADC_BSP.h"
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
#define V25 760.0   // mV
#define AVG_SLOPE 2.5 //mV/°C

#define STEP_ADC  (3000.0/4095)//Опорное напряжение(VREF+)/12бит, mV
/* Private func ------------------------------------------------------------*/
double dAdc_AverageThermo(double dVal);
/* Private variables ---------------------------------------------------------*/
//значение ADC приведенное к напряжению
double Vsense;

//дамп значений
double dTermArr[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
double *pd_TermArr;

//глобальные

u8 ucGotAverage =0; // флаг стартовых 8 измерений
double dCurrentTermo; 

void vAdc_Ini() 
{
  ADC_InitTypeDef ADC_IniStruct;
  ADC_CommonInitTypeDef ADC_Comon_Strct;

  ADC_StructInit(&ADC_IniStruct);
  
  
  ADC_CommonStructInit(&ADC_Comon_Strct);
  /* разрешаем тактирование AЦП1 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  ADC_DeInit();
  
  /* АЦП1 и АЦП2 работают независимо */
  ADC_Comon_Strct.ADC_Mode = ADC_Mode_Independent;
  ADC_Comon_Strct.ADC_Prescaler = ADC_Prescaler_Div2;
  /* выключаем scan conversion */
  ADC_IniStruct.ADC_ScanConvMode = DISABLE;
  /* штучные преобразования */
  ADC_IniStruct.ADC_ContinuousConvMode = DISABLE;
  
  /* Начинать преобразование программно, а не по срабатыванию триггера */
  ADC_IniStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  /* 12 битное преобразование. результат в 12 младших разрядах результата */
  ADC_IniStruct.ADC_DataAlign = ADC_DataAlign_Right;
  
  /* инициализация */
  ADC_CommonInit(&ADC_Comon_Strct);
  
  ADC_Init(ADC1, &ADC_IniStruct);
  /* Включаем АЦП1 */
  ADC_Cmd(ADC1, ENABLE);
  
  // разрешаем канал температурного сенсора
  ADC_TempSensorVrefintCmd(ENABLE); 
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_3Cycles);

  pd_TermArr = dTermArr;
}
//======================================
u16 usAdc_Temperature_read()
{
  
  // начинаем работу
  ADC_SoftwareStartConv(ADC1);
  // ждём пока преобразуется напряжение в код
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // возвращаем результат
  return ADC_GetConversionValue(ADC1);
}

double dAdc_CalculateThermo(u16 ADC_Val)
{

  double Temp_uC;
  Vsense = ADC_Val * STEP_ADC;   //Reading in mV
  /*
  Temperature (in °C) = {(VSENSE – V25) / Avg_Slope} + 25
  
  Где:
  V25 = VSENSE значение для 25° C
  Avg_Slope = шаг значения VSENSE на 1°C в диапазоне температур около 25° C
  В data sheet для STM32F40Xxx значения:
  Avg_Slope - 2,5мВ/°C
  V25 - 760 мВ
  */
  
  
  Temp_uC = ((Vsense- V25) / AVG_SLOPE)+25;
  
  Temp_uC = dAdc_AverageThermo(Temp_uC);
  
  return Temp_uC;
}

void vAdc_Clear_Arr()
{
 memset(dTermArr,0,sizeof(dTermArr));
 pd_TermArr = dTermArr;
 ucGotAverage =0;
}

double dAdc_AverageThermo(double dVal)
{ 
  int i;
  double Sum =0.0;
  *pd_TermArr = dVal;
  
  pd_TermArr++;
  if(pd_TermArr > &dTermArr[8])
  {
     pd_TermArr = dTermArr;
     ucGotAverage =1; // набрали все восемь
  }
  
  for(i = 0; i < 8; i++)
  {
    Sum += dTermArr[i]; 
  }
  return (Sum / 8); 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                               vAdcTask()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void  vAdcTask(void *pvParameters)
{ 
  u16 ADC_Value;
  // счетчик 
  portTickType xLastWakeTime;
  const portTickType xFrequency = 125;
  // Инициализируем xLastWakeTime текущим временем
  xLastWakeTime = xTaskGetTickCount();
  
  while(1)
	 {

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		//GPIOD->BSRRL = GPIO_Pin_3;
	     ADC_Value= usAdc_Temperature_read();
        dCurrentTermo =dAdc_CalculateThermo(ADC_Value);
	   // GPIOD->BSRRH = GPIO_Pin_3;
	 }
  vTaskDelete(NULL);	 
}
//*****************************************************************************
	

