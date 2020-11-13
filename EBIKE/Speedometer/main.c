#include "stm32f4xx.h"                  // Device header

// Constants

const int hall_counts_per_rot = 23; // number of counts per one wheel rotation
const float wheel_diameter = 0.4;//  0.662; //meters
const float pi = 3.1412; // pi
const float cir = pi * wheel_diameter; // circumfrenece of the wheel
const float mps_mph = 2.23694; // m/s to m/h conversion

//define some vars
volatile unsigned char LED_ON; // this is used to toggle the LED
volatile unsigned int HAL_count, HAL_count_frozen, timer_count = 0; // used for counting

double speed = 0.0; // var to hold the speed

//main loop
int main(void){
	
	//Green LED is PD12
	//enable the GPIO PORT D CLK
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	
	//Button is on PA0
	//enable the GPIO port A CLK
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	//EXTI is on syscfg clk, so needs to be enabled
	RCC->AHB2ENR |= RCC_APB2ENR_SYSCFGEN; // en syscfg
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;	//link line 0 with Port A (so we're using PA0 as the interrupt)
	
	//Configure the interupts
	EXTI->IMR |= EXTI_IMR_MR0; // set line 0 to be enabled
	EXTI->RTSR	|= EXTI_RTSR_TR0; // set it to trigger on a rising edge
	
	NVIC_SetPriority(EXTI0_IRQn, 0x03);
	NVIC_EnableIRQ(EXTI0_IRQn);
	
	//Configure the pin 12 as an output
	GPIOD->MODER	|= GPIO_MODER_MODER12_0;		// set the mode as ouput
	
	//Configure the port A pin 0 as an input
	GPIOA->MODER	&= ~(GPIO_MODER_MODER12);
		
	//Configure timer 3 for a 0.5s delay, used for averaging the counts to get a speed reading
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // enable the timer
	TIM3->PSC = 0xF3; // this should give a 0.5s timer
	TIM3->DIER |= TIM_DIER_UIE; //enable timer interrupts
	NVIC_EnableIRQ(TIM3_IRQn); // enable the IRQ
	TIM3->CR1 |= TIM_CR1_CEN; //enable the counter
	
	
	// blink the LED every 0.5s, to check the timer is working
	while(1){
		
		timer_count = TIM3->CNT; // get the value of the counter, used for debugging
		
		speed = ((HAL_count_frozen * cir/hall_counts_per_rot) / 0.5) * mps_mph; // calculate the speed, in mph

		//turn on the LED
		if(LED_ON) {
			GPIOD->ODR	|= GPIO_ODR_ODR_12;		// write a 1 to the output data register to turn on the LED
		} else {
			GPIOD->ODR	&= ~(GPIO_ODR_ODR_12);	// else, write a 0 to the output data register
		}

	}
	
}


//interrupt service routine, for counting pulses
void EXTI0_IRQHandler(void) {
	if(EXTI->PR & (GPIOA->IDR) ) {
		//clear the progress reg
		EXTI->PR = 0x1;
		
		//LED_ON = !LED_ON; // toggle the led
		
		HAL_count++; // inc the counter
	}
}

//interrupt service routine for the timer
int TIM3_IRQHandler(void) {
  TIM3->SR = 0;		// clear the status reg
	//LED_ON = !LED_ON; // toggle the led
	HAL_count_frozen = HAL_count; // freeze the counter
	HAL_count = 0;// reset the counter
	
	return HAL_count_frozen; // return the value for processing outside the IRQ
}

