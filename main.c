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
         		    	   if(wysokosc<78)
         		    	   {
         		    		   UB_Led_On(LED_GREEN);
         		    		   UB_Led_Off(LED_RED);
         		    		   wysokosc=wysokosc+1.6;//else UB_Led_Off(LED_GREEN);
         		    	   }

         		       if(przycisk==68||przycisk==69)
         		    	  if(wysokosc>0)
         		    	  {
         		    	        UB_Led_On(LED_RED);
         		    	        UB_Led_Off(LED_GREEN);
         		    	        wysokosc--;//else UB_Led_Off(LED_RED);
         		    	  }

         			   if(UB_IRMP_Read(&myIRData)==TRUE) //jesli odebrano dane
         			   {
         			      if(myIRData.address==65280)
         			      {
         			    	  // przycisk "<<" - za³¹czenie silnika, zwijanie rolety
         			    		  if(myIRData.command==68) przycisk=68; //UB_Led_On(LED_GREEN);else UB_Led_Off(LED_GREEN);
         			    	  // przycisk ">>" - za³¹czenie silnika, rozwijanie rolety
         			    		  if(myIRData.command==64) przycisk=64; //UB_Led_On(LED_RED);else UB_Led_Off(LED_RED);
         			    		  //przycisk=myIRData.command;
         			    		  //69 CH-
         			    		  //70 CH
         			    		  if(myIRData.command==9)
         			    		  {
         			    		   TIM_Cmd(TIM4, ENABLE);
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

         			   if(wysokosc>=78)
         			   {
         				   UB_Led_Off(LED_GREEN);
         				   wysokosc=78;
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
						 if(ADC_Result<=1.25&&wysokosc<78)
						 {
							UB_Led_On(LED_GREEN);
							wysokosc=wysokosc+1.6;
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

void TIM4_IRQHandler(void)
{
	 	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
         	{
         		if(przycisk!=9) przycisk=9;
         		else przycisk=0;
         		TIM_Cmd(TIM4, DISABLE);

           		// wyzerowanie flagi wyzwolonego przerwania
                TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

                //TIM_Cmd(TIM4, DISABLE);
         	}
}

int main (void)
{
	  SystemInit();
	  UB_Led_Init(); //inicjuje diody LED
	  UB_IRMP_Init(); //inicjuje IR

	  //timer 3 do sterowania rolet¹==============================
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	  TIM_TimeBaseStructure.TIM_Period = 1050-1;
	  TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;
	  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	  TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	  NVIC_InitTypeDef NVIC_InitStructure;
	  // numer przerwania
	  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	  // priorytet g³ówny
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	  // subpriorytet
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	  // uruchom dany kana³
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  // zapisz wype³nion¹ strukturê do rejestrów
	  NVIC_Init(&NVIC_InitStructure);
	  // wyczyszczenie przerwania od timera 3 (wyst¹pi³o przy konfiguracji timera)
	  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	  // zezwolenie na przerwania od przepe³nienia dla timera 3
	  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	  TIM_Cmd(TIM3, ENABLE);
	  //TIM3==========================================================

	  //timer 4 drgania styku===========================
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure2;
	  TIM_TimeBaseStructure2.TIM_Period = 2100-1;
	  TIM_TimeBaseStructure2.TIM_Prescaler = 10000-1;
	  TIM_TimeBaseStructure2.TIM_ClockDivision = TIM_CKD_DIV1;
	  TIM_TimeBaseStructure2.TIM_CounterMode =  TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure2);

	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	  NVIC_InitTypeDef NVIC_InitStructure2;
	  // numer przerwania
	  NVIC_InitStructure2.NVIC_IRQChannel = TIM4_IRQn;
	  // priorytet g³ówny
	  NVIC_InitStructure2.NVIC_IRQChannelPreemptionPriority = 0x00;
	  // subpriorytet
	  NVIC_InitStructure2.NVIC_IRQChannelSubPriority = 0x00;
	  // uruchom dany kana³
	  NVIC_InitStructure2.NVIC_IRQChannelCmd = ENABLE;
	  // zapisz wype³nion¹ strukturê do rejestrów
	  NVIC_Init(&NVIC_InitStructure2);
	  // wyczyszczenie przerwania od timera 3 (wyst¹pi³o przy konfiguracji timera)
	  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	  // zezwolenie na przerwania od przepe³nienia dla timera 3
	  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	  //TIM_Cmd(TIM4, ENABLE);
	  //TIM4==========================================================

	  //ADC do fotorezystora=========================================
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); // zegar dla portu GPIO z którego wykorzystany zostanie pin jako wejœcie ADC (PA1)
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // zegar dla modu³u ADC1

	  GPIO_InitTypeDef GPIO_InitStructure;	  //inicjalizacja wejœcia ADC
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	  // niezale¿ny tryb pracy przetworników
	  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	  // zegar g³ówny podzielony przez 2
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	  // opcja istotna tylko dla trybu multi ADC
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	  // czas przerwy pomiêdzy kolejnymi konwersjami
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInit(&ADC_CommonInitStructure);

	  ADC_InitTypeDef ADC_InitStructure;
	  //ustawienie rozdzielczoœci przetwornika na maksymaln¹ (12 bitów)
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  //wy³¹czenie trybu skanowania (odczytywaæ bêdziemy jedno wejœcie ADC
	  //w trybie skanowania automatycznie wykonywana jest konwersja na wielu //wejœciach/kana³ach)
	  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	  //w³¹czenie ci¹g³ego trybu pracy
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  //wy³¹czenie zewnêtrznego wyzwalania
	  //konwersja mo¿e byæ wyzwalana timerem, stanem wejœcia itd. (szczegó³y w //dokumentacji)
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  //wartoœæ binarna wyniku bêdzie podawana z wyrównaniem do prawej
	  //funkcja do odczytu stanu przetwornika ADC zwraca wartoœæ 16-bitow¹
	  //dla przyk³adu, wartoœæ 0xFF wyrównana w prawo to 0x00FF, w lewo 0x0FF0
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  //liczba konwersji równa 1, bo 1 kana³
	  ADC_InitStructure.ADC_NbrOfConversion = 1;
	  // zapisz wype³nion¹ strukturê do rejestrów przetwornika numer 1
	  ADC_Init(ADC1, &ADC_InitStructure);

	  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

	  ADC_Cmd(ADC1, ENABLE);
      //ADC====================================================

	while(1)
	{
	}
}
