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

void initializeTIM4(void){
	//We need to initialize TIM4 (the timer connected to the 4 LEDs, page 27, Discovery Board Manual)
	//Enable it on the APB1 (page 12, Discovery Board Manual)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	
	//The Timer Initializer
	TIM_TimeBaseInitTypeDef timer_initStructure;
	
	//SystemCoreClock/2 = APB1 frequency, so we want that divided
	//	with a large enough number so it looks continuous
	timer_initStructure.TIM_Prescaler = ((SystemCoreClock/2)/1000000)-1; 
	//Counting upwards
	timer_initStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//Period set to 1000 so it's easier to control the PWM 
	timer_initStructure.TIM_Period = 1000 -1; 
	//No Clock Division (0)
	timer_initStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//No repetition counter (not valid for TIM4)
	timer_initStructure.TIM_RepetitionCounter = 0; 
	
	//Initialize TIM4
	TIM_TimeBaseInit(TIM4, &timer_initStructure); 
	
	//PWM Setup
	//The initializer structure
  TIM_OCInitTypeDef TIM_OCInitStructure;
	//Set the Mode to PWM1
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//Enable the output
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//Initially the Compute Compare Register (CCR) is 0
  TIM_OCInitStructure.TIM_Pulse = 0; 
	//Use Active High so that it starts at 1 and goes to 0 after, for the PWM
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  
	//Configure all 4 channels of PWM1 (Each channel is connected an LED
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	//Start the timer 
	TIM_Cmd(TIM4, ENABLE); 
}

/**
	Sets the PWM length for the motor
	@param pulseLength The length of the PWM 
*/
void setPWMLength(uint16_t pulseLength){
	TIM_SetCompare3(TIM4, pulseLength); 
}
