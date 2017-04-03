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

int main (void)
{
	SystemInit();

	  IRMP_DATA  myIRData;

	  UB_Led_Init(); //inicjuje diody LED

	  UB_IRMP_Init(); //inicjuje IR

	while(1)
	{
	    if(UB_IRMP_Read(&myIRData)==TRUE) //jesli odebrano dane
	    {
	      if(myIRData.address==65280)
	      {
	    	  // przycisk "<<" - za³¹czenie silnika, zwijanie rolety
	    		  if(myIRData.command==68) UB_Led_On(LED_GREEN);else UB_Led_Off(LED_GREEN);
	    	  // przycisk ">>" - za³¹czenie silnika, rozwijanie rolety
	    		  if(myIRData.command==64) UB_Led_On(LED_RED);else UB_Led_Off(LED_RED);
	      }
	    }
	}
}
