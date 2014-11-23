#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define MOTOR GPIO_Pin_11 

extern void updateMotor(float temperature);

int length; 

/* Initializes the Servo Motor */ 
void initializeMotor(void) {
	//define the initializer struct 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Pass in the struct
	GPIO_StructInit(&GPIO_InitStructure);
	
	//Set the gpio speed to 50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//D11 is a free pin
	GPIO_InitStructure.GPIO_Pin = MOTOR;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/* Update the motor position with a given temperature */
void updateMotor(float temperature){	
	//Calculate the length that we need to keep the bit high
	length = temperature * 850 - 4000;

	//Turn on the motor
	GPIO_SetBits(GPIOD, MOTOR);
	
	int count = 0; 
	while(count++ < length);
	
	//Turn off the motor
	GPIO_ResetBits(GPIOD, MOTOR);
}
