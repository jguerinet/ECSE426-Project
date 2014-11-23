#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

//Define the 4 LEDs we are going to be switching on and off 
#define LED_GREEN GPIO_Pin_12
#define LED_ORANGE GPIO_Pin_13
#define LED_RED GPIO_Pin_14
#define LED_BLUE GPIO_Pin_15

void resetVariables(void);

int currentLED; 
int count; 

/* Initializes the LEDs for the overheating alarm */
void initializeLEDs(void) {
	//Define the initializer struct 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Pass in the struct
	GPIO_StructInit(&GPIO_InitStructure);
	
	//Initial settings
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	//Initialize green LED
	GPIO_InitStructure.GPIO_Pin = LED_GREEN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//Initialize orange LED
	GPIO_InitStructure.GPIO_Pin = LED_ORANGE;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//Initialize red LED
	GPIO_InitStructure.GPIO_Pin = LED_RED;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//Initialize blue LED
	GPIO_InitStructure.GPIO_Pin = LED_BLUE;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//Set up the variables
	resetVariables();
}

/* Turn the LED off or on, and change color if needed*/
void switchLED(int on){
	//Increment count
	count ++;
	
	//If we need to turn off the alarm, turn off the current LED and reset everything
	if(!on){
		GPIO_ResetBits(GPIOD, currentLED);
		resetVariables();
	}
	//If we are still keeping the LEDs on, check if we need to switch 
	else if(count == 5){
		//Reset count
		count = 0; 

		//Turn off the current LED
		GPIO_ResetBits(GPIOD, currentLED);
		
		//Go to the next LED
		switch(currentLED){
			case LED_ORANGE: 
				currentLED = LED_RED;
				break;
			case LED_RED:
				currentLED = LED_BLUE; 
				break;
			case LED_BLUE:
				currentLED = LED_GREEN; 
				break;
			case LED_GREEN: 
				currentLED = LED_ORANGE; 
				break;
		}
		
		//Turn on the next LED
		GPIO_SetBits(GPIOD, currentLED);
	}
}

/* Reset the variables to their initial values */
void resetVariables(void){
	//Reset the default current LED
	currentLED = LED_ORANGE;
	
	//Reset the count to 0
	count = 0; 
}


