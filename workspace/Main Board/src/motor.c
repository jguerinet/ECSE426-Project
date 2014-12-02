#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define MIN_PERIOD_LENGTH 0
#define MAX_PERIOD_LENGTH 1000
#define MAX_DUTY_CYCLE 		4

void initializeTIM4();

uint8_t dutyCycle; 
uint16_t periodLength;
//Boolean to keep track of whether we are increasing or decreasing the angle
int increasingAngle; 

/* Initializes the Servo Motor */ 
void initializeMotor(void) {
	//define the initializer struct 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Pass in the struct
	GPIO_StructInit(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Set the duty cycle to 0
	dutyCycle = 0; 
	
	//Set the period length to the min
	periodLength = MIN_PERIOD_LENGTH;
	
	//Initially increasing the angle
	increasingAngle = 1; 
	
	//Initialize the timer
	initializeTIM4(); 
}

/* Initializes the timer for the motor */
void initializeTIM4(void){
	//Enable it on the APB1 (page 14, User Manual)
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
  
	//Configure channel 1
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

/**
	Updates the motor angle
*/
void updateMotor(void){
	//Increment the duty cycle
	dutyCycle ++; 
	
	//Check if we need to increment the period length 
	if(dutyCycle == MAX_DUTY_CYCLE){
		//Reset the duty Cycle to 0 
		dutyCycle = 0; 
		
		//Check if we have reached one of the ends
		if(periodLength == MAX_PERIOD_LENGTH){
				//Max period length achieved, so switch to decreasing angle
				increasingAngle = 0; 
		}
		else if(periodLength == MIN_PERIOD_LENGTH){
				//Min period length achieved, so switch to increasing angle
				increasingAngle = 1; 
		}
		
		//If we are increasing the angle, increment the period
		if(increasingAngle){
			periodLength ++; 
		}
		//If we are decreasing the angle, decrement the period
		else{
			periodLength --; 
		}
		
		//Set the new PWM length 
		TIM_SetCompare1(TIM4, periodLength); 
	}
}

/**
	Reads the angle of the motor
	@return The motor angle
*/
uint8_t getMotorAngle(void){
	//TODO
	return 0; 
}
