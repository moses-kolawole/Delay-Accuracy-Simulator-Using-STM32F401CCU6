#include "main.h"

uint32_t software_time = 0;
uint32_t software_start = 0;
uint32_t software_end = 0;

uint32_t hardware_time = 0;
uint32_t hardware_start = 0;
uint32_t hardware_end = 0;

uint32_t delay_comparer = 0;

int main(void)
{
	//Enable the GPIOA Clock
	RCC->AHB1ENR |= (1 << 0);

	//Set PA6 as Output
	GPIOA->MODER &= ~(3 << (6 * 2));
	GPIOA->MODER |=  (1 << (6 * 2));

	//Enable TIM2 Clock
	RCC->APB1ENR |= (1 << 0);

	//Configure TIM2
	TIM2->PSC = 15;          //16 MHz /16 = 1 MHz
	TIM2->ARR = 0xFFFFFFFF;  //Maximum Count

	//Reset Counter
	TIM2->CNT = 0;

	//Enable TIM2
	TIM2->CR1 |= (1 << 0);

	while(1)
	{
		/**************************************************
		 * SOFTWARE DELAY MEASUREMENT
		 **************************************************/

		//Reset Timer
		TIM2->CNT = 0;

		//Record Start Time
		software_start = TIM2->CNT;

		//Software Delay
		for(volatile uint32_t i = 0; i < 50000; i++);

		//Record End Time
		software_end = TIM2->CNT;

		//Calculate Software Time
		software_time = software_end - software_start;


		/**************************************************
		 * HARDWARE DELAY MEASUREMENT
		 **************************************************/

		//Reset Timer
		TIM2->CNT = 0;

		//Record Start Time
		hardware_start = TIM2->CNT;

		//Hardware Delay (Wait for TIM2 to reach 50000 counts)
		while(TIM2->CNT < 50000);

		//Record End Time
		hardware_end = TIM2->CNT;

		//Calculate Hardware Time
		hardware_time = hardware_end - hardware_start;


		/**************************************************
		 * COMPARE BOTH DELAYS
		 **************************************************/

		if(hardware_time > software_time)
		{
			delay_comparer = hardware_time - software_time;
		}
		else
		{
			delay_comparer = software_time - hardware_time;
		}


		/**************************************************
		 * DISPLAY RESULT
		 **************************************************/

		//Difference less than 100 timer counts
		if(delay_comparer > 100)
		{
			//Delay is Accurate
			GPIOA->ODR |= (1 << 6);
		}
		else
		{
			//Delay is not Accurate
			GPIOA->ODR &= ~(1 << 6);
		}
	}

	return 0;
}
