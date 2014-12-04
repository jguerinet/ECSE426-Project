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

/* MOTOR SPEED */
extern uint8_t getMotorAngle(void); 
int ticks; 

void testMotorSpeed(void){
	//Initialize the ticks to 0
	ticks = 0;
	int extremity = 0; 
	
	initializeMotor(); 
	initializeProximitySensor(); 
	
	//Note: Change the speed in the motor.c file
	
	//Main Loop
	//if(SysTick_Config(20*SystemCoreClock/1000) == 0) {
		while(!extremity){
			if(ticks){
				ticks = 0;
			
				//Moving Average filter for the distances
				Filter filter;
				 uint32_t filter_buffer[20];
				filter_init(&filter, (int32_t*)filter_buffer, 20); 
				
					//Moving Average filter for the angles
				Filter angleFilter;
				 uint32_t angle_filter_buffer[20];
				filter_init(&angleFilter, (int32_t*)angle_filter_buffer, 20); 
					
				for(int i = 0; i < 10; i++){
					//Get the measured distance from the sensor
					uint16_t measuredDistance = getSensorDistance(); 
					
					//If it's not 0, add it to the filter and the angle
					if(measuredDistance != 0){
						filter_add(&filter, measuredDistance);
						
						filter_add(&angleFilter, getMotorAngle()); 
					}
				}
				
				//Update the motor angle
				int extremity = updateMotor(); 
				
				//If we've reached the end, set the average position
				if(extremity){
					uint8_t distance = (uint8_t)filter_avg(&filter);  
					printf("Distance: %d\n", distance); 
					//Get the angle from the filter
					uint8_t angle = (uint8_t)filter_avg(&angleFilter); 
					printf("Angle: %d\n", angle); 
					
					break; 
				}
			}
		}
	//}
}
