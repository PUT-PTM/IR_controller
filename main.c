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

int przycisk=0;
IRMP_DATA  myIRData;

void TIM3_IRQHandler(void)
{
         	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
         	{
         		       if(przycisk==68) UB_Led_On(LED_GREEN); //else UB_Led_Off(LED_GREEN);
         			   if(przycisk==64) UB_Led_On(LED_RED); //else UB_Led_Off(LED_RED);

         			   if(UB_IRMP_Read(&myIRData)==TRUE) //jesli odebrano dane
         			    {
         			      if(myIRData.address==65280)
         			      {
         			    	  // przycisk "<<" - za³¹czenie silnika, zwijanie rolety
         			    		  if(myIRData.command==68) przycisk=68; //UB_Led_On(LED_GREEN);else UB_Led_Off(LED_GREEN);
         			    	  // przycisk ">>" - za³¹czenie silnika, rozwijanie rolety
         			    		  if(myIRData.command==64) przycisk=64; //UB_Led_On(LED_RED);else UB_Led_Off(LED_RED);
         			      }
         			    }
         			    else {
         					przycisk=0;
         					UB_Led_Off(LED_GREEN);
         					UB_Led_Off(LED_RED);
         				}

                	// wyzerowanie flagi wyzwolonego przerwania
                	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
         	}
}

int main (void)
{
	SystemInit();

	  UB_Led_Init(); //inicjuje diody LED

	  UB_IRMP_Init(); //inicjuje IR

	  //timer 4 do sterowania rolet¹==============================
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

	while(1)
	{
	}
}
