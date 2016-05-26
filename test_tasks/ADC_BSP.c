/**
******************************************************************************
* @file    ADC_BSP.c 
* @author  Logunov Viacheslav
* @date    
* @brief  ��������� ����������� ���������  
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
#define AVG_SLOPE 2.5 //mV/�C

#define STEP_ADC  (3000.0/4095)//������� ����������(VREF+)/12���, mV
/* Private func ------------------------------------------------------------*/
double dAdc_AverageThermo(double dVal);
/* Private variables ---------------------------------------------------------*/
//�������� ADC ����������� � ����������
double Vsense;

//���� ��������
double dTermArr[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
double *pd_TermArr;

//����������

u8 ucGotAverage =0; // ���� ��������� 8 ���������
double dCurrentTermo; 

void vAdc_Ini() 
{
  ADC_InitTypeDef ADC_IniStruct;
  ADC_CommonInitTypeDef ADC_Comon_Strct;

  ADC_StructInit(&ADC_IniStruct);
  
  
  ADC_CommonStructInit(&ADC_Comon_Strct);
  /* ��������� ������������ A��1 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  ADC_DeInit();
  
  /* ���1 � ���2 �������� ���������� */
  ADC_Comon_Strct.ADC_Mode = ADC_Mode_Independent;
  ADC_Comon_Strct.ADC_Prescaler = ADC_Prescaler_Div2;
  /* ��������� scan conversion */
  ADC_IniStruct.ADC_ScanConvMode = DISABLE;
  /* ������� �������������� */
  ADC_IniStruct.ADC_ContinuousConvMode = DISABLE;
  
  /* �������� �������������� ����������, � �� �� ������������ �������� */
  ADC_IniStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  /* 12 ������ ��������������. ��������� � 12 ������� �������� ���������� */
  ADC_IniStruct.ADC_DataAlign = ADC_DataAlign_Right;
  
  /* ������������� */
  ADC_CommonInit(&ADC_Comon_Strct);
  
  ADC_Init(ADC1, &ADC_IniStruct);
  /* �������� ���1 */
  ADC_Cmd(ADC1, ENABLE);
  
  // ��������� ����� �������������� �������
  ADC_TempSensorVrefintCmd(ENABLE); 
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_3Cycles);

  pd_TermArr = dTermArr;
}
//======================================
u16 usAdc_Temperature_read()
{
  
  // �������� ������
  ADC_SoftwareStartConv(ADC1);
  // ��� ���� ������������� ���������� � ���
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // ���������� ���������
  return ADC_GetConversionValue(ADC1);
}

double dAdc_CalculateThermo(u16 ADC_Val)
{

  double Temp_uC;
  Vsense = ADC_Val * STEP_ADC;   //Reading in mV
  /*
  Temperature (in �C) = {(VSENSE � V25) / Avg_Slope} + 25
  
  ���:
  V25 = VSENSE �������� ��� 25� C
  Avg_Slope = ��� �������� VSENSE �� 1�C � ��������� ���������� ����� 25� C
  � data sheet ��� STM32F40Xxx ��������:
  Avg_Slope - 2,5��/�C
  V25 - 760 ��
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
     ucGotAverage =1; // ������� ��� ������
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
  // ������� 
  portTickType xLastWakeTime;
  const portTickType xFrequency = 125;
  // �������������� xLastWakeTime ������� ��������
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
	

