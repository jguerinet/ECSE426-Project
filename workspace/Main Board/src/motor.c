#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define MIN_PULSE_LENGTH 23
#define MAX_PULSE_LENGTH 98

void initializeTIM2(void);

uint16_t pulseLength;
//Boolean to keep track of whether we are increasing or decreasing the angle
int increasingAngle; 

/* Initializes the Servo Motor */ 
void initializeMotor(void) {
	//define the initializer struct 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Pass in the struct
	GPIO_StructInit(&GPIO_InitStructure);
	
	//GPIO Config
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	//GPIO Init
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Config to use TIM2 (Page 21, Discovery Board Manual)
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2); 
	
	//Set the period length to the min
	pulseLength = MIN_PULSE_LENGTH;
	
	//Initially increasing the angle
	increasingAngle = 1; 
	
	//Initialize the timer
	initializeTIM2(); 
}

/* Initializes the timer for the motor */
void initializeTIM2(void){
	//Enable it on the APB1 (page 14, User Manual)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	
	//The Timer Initializer
	TIM_TimeBaseInitTypeDef timer_initStructure;
	
	timer_initStructure.TIM_Prescaler = ((SystemCoreClock/2)/40000)-1; 
	//Counting upwards
	timer_initStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timer_initStructure.TIM_Period = 1000-1; 
	//No Clock Division (0)
	timer_initStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//No repetition counter (not valid for TIM4)
	timer_initStructure.TIM_RepetitionCounter = 0; 
	
	//Initialize TIM2
	TIM_TimeBaseInit(TIM2, &timer_initStructure); 
	
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
  
	//Configure channel 1 (pg 21, Discovery Board Manual)
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	//Start the motor timer 
	TIM_Cmd(TIM2, ENABLE); 
}

/**
	Updates the motor angle
*/
void updateMotor(void){ 
	//Check if we have reached one of the ends
	if(pulseLength == MAX_PULSE_LENGTH){
			//Max period length achieved, so switch to decreasing angle
			increasingAngle = 0; 
	}
	else if(pulseLength == MIN_PULSE_LENGTH){
			//Min period length achieved, so switch to increasing angle
			increasingAngle = 1; 
	}
	
	//If we are increasing the angle, increment the pulse
	if(increasingAngle){
		pulseLength += 1; 
	}
	//If we are decreasing the angle, decrement the pulse
	else{
		pulseLength -= 1; 
	}
	
	//Set the new PWM length 
	TIM_SetCompare1(TIM2, pulseLength); 
}

/**
	Reads the angle of the motor
	@return The motor angle
*/
uint8_t getMotorAngle(void){
	uint8_t angle = ((pulseLength - MIN_PULSE_LENGTH) * 180) / (MAX_PULSE_LENGTH - MIN_PULSE_LENGTH); 
	
	//printf("Angle: %d\n", angle); 
	
	return angle; 
}
