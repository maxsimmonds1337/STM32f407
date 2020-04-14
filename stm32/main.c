#include <stm32f4xx.h>

void delay (void) 				//create simple delay loop
{
	int d;
	for (d=0; d<100000; d++) {
		__asm("NOP");
	}
}

 int addr = 0x67;  // I2C address of the MPU6050
 char res;

int main (void) {
		 //delay(); // needed to wait for the I2C perh. to come up

	// I2C setup
	//I2C1->CR1 &= ~(0x1); // turn off the peripheral
	// set up GPIO port B
	RCC->AHB1ENR |= 0x2; // enables the GPIO B clock
	RCC->APB1ENR |= (0x1) << 21;  // (bit 21 enables the i2c1 clock
	
	// mode selection
	GPIOB->MODER |= 0x2 << 12; // set port B6 as i2c for its AF
	GPIOB->MODER |= 0x2 << 14; // set port B7 as i2c for its AF
	
	//output type (open drain for I2C)
	GPIOB->OTYPER |= 0x1 << 6; // B6 as open drain
	GPIOB->OTYPER |= 0x1 << 7; // B7 as open drain
	
	//output speed is low speed, for all io on port B
	//GPIOB->OSPEEDR |= 0;
	
	GPIOB->PUPDR &= ~(3<<12); // no pull ups/downs as we already have them on the mp6050
	GPIOB->PUPDR &= ~(3<<14);
	// set the gpios as AF
	GPIOB->AFR[0]	|= (0x4) << 24;  // This sets PB7 alternate function 4 (I2C 1)
	GPIOB->AFR[0] |= (0x4) << 28;				// this sets PB8	(I2C 1)
	
	//I2C reg stuff
	 
	I2C1->CR2 |= 0x0008; // Set the peripheral clock to 8MHz


	I2C1->CCR |= 0x0028; // explained here http://webcache.googleusercontent.com/search?q=cache:JroCGyH4tEIJ:tath.eu/projects/stm32/stm32-i2c-calculating-clock-control-register/+&cd=4&hl=en&ct=clnk&gl=uk
	I2C1->TRISE |= 0x009;  // as above

	I2C1->CR1 = 0x0001; // turn on the peripheral
	
	//RCC->AHB1RSTR |= 0x1 << 21; // reset the i2c
	
	
	delay(); // needed to wait for the I2C perh. to come up


	// LED set up
 //RCC->AHB1ENR|=0x8; 			//IO PortD clock enable

 //GPIOD->MODER |= 0x01000000; 	//Set GPIOD12 as output PIN
 //GPIOD->OTYPER = 0; 			//Set output as Push-Pull mode
 //GPIOD->OSPEEDR = 0; 		//Set output speed 2MHz low speed
 //GPIOD->PUPDR = 0; 			//Set no pull up and pull down

 //start = I2C->CR1; // mask the start bit 	
  	 //GPIOD->ODR |= 0x1000;                 	//Set GPIOD12 high only, turn on LED6	delay();

 while(1) {

	 //start
	 I2C1->CR1 |= 1<<8; // send start bit
   while (!(I2C1->SR1 & 0x0001));// //wait for start bit,

	 //send addr
	 I2C1->DR = 0x5; // send address of MPU-6050
	 //while (!(I2C1->SR1 & 0x0002));	 // wait for not busy bit
	 res = (I2C1->SR2);	 // dummy read to clear addr flag
	 
	 //write
 	 //I2C1->DR = 'A'; // send 'A' via I2C;
	 //while (!(I2C1->SR1 & (1<<7)));  //wait for data register to be empty
	 
	 
   //stop
	 I2C1->CR1 |= 0x0200; // send stop bit
	 
	 //__asm("NOP");
	 delay();
	 //GPIOD->ODR &= ~(0x1);			// Turns LED off
	 //__asm("NOP");
 }	

}
