#include "stm32f4xx.h"                  // Device header

volatile unsigned char LED_ON; // this is used to toggle the LED
volatile unsigned int count = 0; // used for counting

int main(void){
	
	// turn on an LED when button is pressed (PA0), via interrupts
	while(1){
		
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
		
		//If the button is pressed, then turn on the light
		if(LED_ON) {
			GPIOD->ODR	|= GPIO_ODR_ODR_12;		// write a 1 to the output data register to turn on the LED
		} else {
			GPIOD->ODR	&= ~(GPIO_ODR_ODR_12);	// else, write a 0 to the output data register
		}
	}
}

void EXTI0_IRQHandler(void) {
	if(EXTI->PR & (GPIOA->IDR) ) {
		EXTI->PR = 0x1;
		LED_ON = !LED_ON; // toggle the led
		
		count++; // inc the counter
	}
}

