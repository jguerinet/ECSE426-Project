/** This file includes the demos for all of the individual parts */
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

//Helper function to simulate delay
static void delay(__IO uint32_t nCount){
  __IO uint32_t index = 0; 
  for(index = 100000*nCount; index != 0; index--);
}

/** MOTOR **/
extern void initializeMotor(void); 
extern void updateMotor(void);

void testMotor(void){
	//Initialize the motor
	initializeMotor();
	
	//In an infinite loop, make the motor rotate 
	while(1){
		//Rotate the motor
		updateMotor(); 
		//Delay
		delay(15); 
	}
}
