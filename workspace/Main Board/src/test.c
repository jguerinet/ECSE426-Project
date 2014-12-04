/** This file includes the demos for all of the individual parts */
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "filter.h"

//Helper function to simulate delay
static void delay(__IO uint32_t nCount){
  __IO uint32_t index = 0; 
  for(index = 100000*nCount; index != 0; index--);
}

/** MOTOR **/
extern void initializeMotor(void); 
extern int updateMotor(void);

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

/** PROXIMITY SENSOR **/
extern void initializeProximitySensor(void); 
extern uint8_t getSensorDistance(void); 

void testProximitySensor(void){
	//Initialize the proximity sensor
	initializeProximitySensor(); 
	
	//In an infinite loop, read the sensor values
	while(1){
		//Read the values 
		uint8_t distance = getSensorDistance(); 
		
		//Print 
		printf("Distance: %d\n", distance); 
		
		//Delay
		delay(15); 
	}
}

/** LCD SCREEN **/
extern void initializeLCD(void); 
extern void clearPosition(uint16_t x, uint16_t y); 
extern void drawPosition(uint16_t x, uint16_t y); 
extern void drawGrid(void); 

void testLCD(void){
	//Initialize the proximity sensor
	initializeLCD(); 
	
	int16_t x = 230; 
	int16_t y = 320; 
	
	//In an infinite loop, show the point at every position
	while(1){
		for(int i = 10; i <= 230; i += 10){
			for(int j = 10; j <= 320; j += 10){
				//Clear the previous position
				clearPosition(x, y); 
				
				//Draw the grid lines
				drawGrid(); 
				
				//Get the new position
				x = i; 
				y = j; 
				
				//Draw the new position
				drawPosition(x, y); 
				
				//Delay
				delay(15); 
			}
		}
	}
}
