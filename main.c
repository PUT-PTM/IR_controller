#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_spi.h"
#include "math.h"
#include "main.h"
#include "stm32_ub_irmp.h"
#include "stm32_ub_led.h"

float wysokosc=0;
int przycisk=0;
float ADC_Result=5;
IRMP_DATA  myIRData;

void TIM3_IRQHandler(void)
{
         	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
         	{
         		       if(przycisk==64||przycisk==70)
         		    	   if(wysokosc<56)
         		    	   {
         		    		   UB_Led_On(LED_GREEN);
         		    		   wysokosc=wysokosc+1.5;//else UB_Led_Off(LED_GREEN);
         		    	   }

         		       if(przycisk==68||przycisk==69)
         		    	  if(wysokosc>0)
         		    	  {
         		    	        UB_Led_On(LED_RED);
         		    	        wysokosc--;//else UB_Led_Off(LED_RED);
         		    	  }

         			   if(UB_IRMP_Read(&myIRData)==TRUE) //jesli odebrano dane
         			   {
         			      if(myIRData.address==65280)
         			      {
         			    	  // przycisk "<<" - za��czenie silnika, zwijanie rolety
         			    		  if(myIRData.command==68) przycisk=68; //UB_Led_On(LED_GREEN);else UB_Led_Off(LED_GREEN);
         			    	  // przycisk ">>" - za��czenie silnika, rozwijanie rolety
         			    		  if(myIRData.command==64) przycisk=64; //UB_Led_On(LED_RED);else UB_Led_Off(LED_RED);
         			    		  //przycisk=myIRData.command;
         			    		  //69 CH-
         			    		  //70 CH
         			    		  if(myIRData.command==9)
         			    			  {
         			    			  if(przycisk!=9) przycisk=9;
         			    			  else przycisk=0;
         			    			  }
         			    		 if(myIRData.command==70) przycisk=70;
         			    		 if(myIRData.command==69) przycisk=69;
         			      }
         			    }
         			    else if(przycisk!=9&&przycisk!=70&&przycisk!=69)
         			    {
         					przycisk=0;
         					UB_Led_Off(LED_GREEN);
         					UB_Led_Off(LED_RED);
         				}

         			   if(wysokosc>=56)
         			   {
         				   UB_Led_Off(LED_GREEN);
         				   wysokosc=56;
         			   }
           			   if(wysokosc<=0)
           			   {
           				   UB_Led_Off(LED_RED);
           				   wysokosc=0;
           			   }
           			   if(przycisk==9)
           			   {
           				 ADC_SoftwareStartConv(ADC1);
           				 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
						 ADC_Result = (ADC_GetConversionValue(ADC1)*2.95)/4095;
						 UB_Led_On(LED_BLUE);

						 //ponizej 1.25 rozwija sie
						 if(ADC_Result<=1.25&&wysokosc<56)
						 {
							UB_Led_On(LED_GREEN);
							wysokosc=wysokosc+1.5;
						 }
						 else UB_Led_Off(LED_GREEN);
						 if(ADC_Result>2.0&&wysokosc>0)
						 {
							UB_Led_On(LED_RED);
							wysokosc--;
						 }
						 else UB_Led_Off(LED_RED);
           			   }
           			   else UB_Led_Off(LED_BLUE);

           		   	// wyzerowanie flagi wyzwolonego przerwania
                	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
         	}
}

int main (void)
{
	  SystemInit();
	  UB_Led_Init(); //inicjuje diody LED
	  UB_IRMP_Init(); //inicjuje IR

	  //timer 4 do sterowania rolet�==============================
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	  TIM_TimeBaseStructure.TIM_Period = 1050-1;
	  TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;
	  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	  TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	  //==========================================================

	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	  NVIC_InitTypeDef NVIC_InitStructure;
	  // numer przerwania
	  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	  // priorytet g��wny
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	  // subpriorytet
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	  // uruchom dany kana�
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  // zapisz wype�nion� struktur� do rejestr�w
	  NVIC_Init(&NVIC_InitStructure);
	  // wyczyszczenie przerwania od timera 3 (wyst�pi�o przy konfiguracji timera)
	  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	  // zezwolenie na przerwania od przepe�nienia dla timera 3
	  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	  TIM_Cmd(TIM3, ENABLE);

	  //ADC=================================================
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); // zegar dla portu GPIO z kt�rego wykorzystany zostanie pin jako wej�cie ADC (PA1)
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // zegar dla modu�u ADC1

	  GPIO_InitTypeDef GPIO_InitStructure;	  //inicjalizacja wej�cia ADC
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	  // niezale�ny tryb pracy przetwornik�w
	  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	  // zegar g��wny podzielony przez 2
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	  // opcja istotna tylko dla trybu multi ADC
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	  // czas przerwy pomi�dzy kolejnymi konwersjami
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInit(&ADC_CommonInitStructure);

	  ADC_InitTypeDef ADC_InitStructure;
	  //ustawienie rozdzielczo�ci przetwornika na maksymaln� (12 bit�w)
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  //wy��czenie trybu skanowania (odczytywa� b�dziemy jedno wej�cie ADC
	  //w trybie skanowania automatycznie wykonywana jest konwersja na wielu //wej�ciach/kana�ach)
	  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	  //w��czenie ci�g�ego trybu pracy
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  //wy��czenie zewn�trznego wyzwalania
	  //konwersja mo�e by� wyzwalana timerem, stanem wej�cia itd. (szczeg�y w //dokumentacji)
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  //warto�� binarna wyniku b�dzie podawana z wyr�wnaniem do prawej
	  //funkcja do odczytu stanu przetwornika ADC zwraca warto�� 16-bitow�
	  //dla przyk�adu, warto�� 0xFF wyr�wnana w prawo to 0x00FF, w lewo 0x0FF0
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  //liczba konwersji r�wna 1, bo 1 kana�
	  ADC_InitStructure.ADC_NbrOfConversion = 1;
	  // zapisz wype�nion� struktur� do rejestr�w przetwornika numer 1
	  ADC_Init(ADC1, &ADC_InitStructure);

	  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

	  ADC_Cmd(ADC1, ENABLE);

	while(1)
	{

	}
}
