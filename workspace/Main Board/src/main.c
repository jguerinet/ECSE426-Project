#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  		// Device header

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include "background16bpp.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265

/* AREA SIZE (cm) */
#define MAX_X 400
#define MAX_Y	400

/* SIGNALS */
//Display Update Signal 
#define DISPLAY_SIGNAL (int32_t) 0x01
//Proximity Sensor Signal
#define PROXIMITY_SENSOR_SIGNAL (int32_t) 0x02

/* METHODS FROM OTHER FILES */
//Proximity Sensor
extern void initializeProximitySensor(void); 
extern uint8_t getSensorDistance(void); 
//Motor
extern void initializeMotor(void);
extern void updateMotor(void); 
extern uint8_t getMotorAngle(void); 

/* THREAD FUNCTIONS */
//LCD
void lcd(void const *arg);
//Proximity Sensor
void proximitySensor(void const *arg); 
//Wireless
void wireless(void const *arg); 

/* THREAD DEFINITIONS */
//LCD
osThreadDef(lcd, osPriorityNormal, 1, 0);
//Proximity Sensor
osThreadDef(proximitySensor, osPriorityHigh, 1, 0); 
//Wireless
osThreadDef(wireless, osPriorityNormal, 1, 0);

/* THREAD IDs */
//LCD
osThreadId lcd_thread;
//Proximity Sensor
osThreadId proximity_sensor_thread; 
//Wireless
osThreadId wireless_thread; 

/* MUTEX DEFINTIIONS */
//Position measured by the sensor
osMutexDef(sensorCoordinates);
//Position measured by the wireless setup 
osMutexDef(wirelessCoordinates); 

/* MUTEX IDs */
osMutexId sensorCoordinatesId;
osMutexId wirelessCoordinatesId; 

/* TIMER FUNCTIONS */
//Display
void displayTimerCallback(void const* arg);
//Proximity Sensor
void proximitySensorTimerCallback(void const* arg);

/* TIMERS */
//Display
osTimerDef(displayTimer, displayTimerCallback); 
//Proximity Sensor
osTimerDef(proximitySensorTimer, proximitySensorTimerCallback); 

//This struct will contain x and y coordinates measured
typedef struct{
	float x; 
	float y;
} Coordinates; 

//This will contain the coordinates measured by the proximity sensor 
Coordinates sensorCoordinates = {-1, -1};
//This will contain the coordinates measured by the boards
Coordinates wirelessCoordinates = {-1, -1};

//Helper method to simulate a delay
static void delay(__IO uint32_t nCount){
  __IO uint32_t index = 0; 
  for(index = 100000*nCount; index != 0; index--){}
}

int main (void) {
	//Initialize CMSIS-RTOS
  osKernelInitialize ();                    
	
	/* LCD */
	//LCD Initialization
  LCD_Init();
  //LCD Layer Initialization
  LCD_LayerInit();
  //Enable the LTDC controller
  LTDC_Cmd(ENABLE);
  //Set LCD foreground layer as the current layer
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
	//Initialize GPIO A Board
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	/* PROXIMITY SENSOR */
	initializeProximitySensor(); 
	
	/* MOTOR */
	initializeMotor(); 
	
	/* Mutex Creations */
	sensorCoordinatesId = osMutexCreate(osMutex(sensorCoordinates));
	wirelessCoordinatesId = osMutexCreate(osMutex(wirelessCoordinates));
	
	/* Thread Creations */
	lcd_thread = osThreadCreate(osThread(lcd), NULL);
	proximity_sensor_thread = osThreadCreate(osThread(proximitySensor), NULL); 
	wireless_thread = osThreadCreate(osThread(wireless), NULL); 
	
	//Start thread execution
	osKernelStart();    

	osDelay(osWaitForever);
}

/**
	* The LCD screen thread. 
  */
void lcd(void const *arg){
	//Clear the LCD
	LCD_Clear(LCD_COLOR_WHITE);
	LCD_SetFont(&Font8x8);
	
	//Draw the grid in blue
	LCD_SetTextColor(LCD_COLOR_BLUE2);
	int i = 0;
	int j = 0;
	//Horizontal Lines
	for (i = 10; i <= 310; i = i+60){
		LCD_DrawLine(10, i, 220, LCD_DIR_HORIZONTAL);
	}
	//Vertical lines 
	for (j = 10; j <= 230; j = j+44){
		LCD_DrawLine(j, 10, 300, LCD_DIR_VERTICAL);
	}
	
	//This will keep track of the x and y coordinates that are currently mapped 
	int16_t xMapped = -1; 
	int16_t yMapped = -1;
	
	//This will keep track of the received x and y coordinates
	int16_t x = -1; 
	int16_t y = -1; 
	
	//Set up the timer and start it
	osTimerId displayTimerId = osTimerCreate(osTimer(displayTimer), osTimerPeriodic, NULL); 
	osTimerStart(displayTimerId, 200); 
	
	//Main Loop
	while(1){
		//Wait until the display signal is set
		osSignalWait(DISPLAY_SIGNAL, osWaitForever);
		
		//Get the sensor coordinates mutex
		osMutexWait(sensorCoordinatesId, osWaitForever); 
		
		//Get the coordinates from this
		x = sensorCoordinates.x;
		y = sensorCoordinates.y;
		
		//Release the sensor coordinates mutex
		osMutexRelease(sensorCoordinatesId); 
		
		//If the coordinates are -1, then the sensor is not picking up on anything
		//Therefore use the wireless coordinates
		if(x == -1){
			//Get the wireless coordinates mutex
			osMutexWait(wirelessCoordinatesId, osWaitForever); 
			
			//Get the wireless coordinates from this
			x = wirelessCoordinates.x; 
			y = wirelessCoordinates.y; 
			
			//Release the wireless coordinates mutex
			osMutexRelease(wirelessCoordinatesId); 
		}
		else{
			//The sensor coordinates assume that the center is x = 0, 
			//	so adjust it to have only x positive values
			x += MAX_X / 2;
		}
		
		//If there was a previous position, hide it
		if(xMapped != -1){
			LCD_SetTextColor(LCD_COLOR_WHITE);
			LCD_DrawFullCircle(xMapped, yMapped, 4);
		}
		
		//Check if there is a new position
		if(x == -1 || y == -1){
			//If there isn't one, continue
			xMapped = -1; 
			yMapped = -1;
			continue; 
		}
		
		//Calculate the new x and y positions to map
		xMapped = (x * 220 / MAX_X) + 10;
		yMapped = (y * 300/ MAX_Y) + 10;
		
		//Show the position on the screen if it is in bounds and existant
		if (xMapped > 9 && xMapped < 231 & yMapped > 9 && yMapped < 311){
			LCD_SetTextColor(LCD_COLOR_RED);
			LCD_DrawFullCircle(xMapped, yMapped, 4);
			delay(125); 
		}
		else{
			//Printf error message
			printf("Out of Bounds: %d, %d\n", xMapped, yMapped);
			//Set the mapped values to -1
			xMapped = -1; 
			yMapped = -1; 
		}
	}
}

/**
	The proximity sensor thread
*/
void proximitySensor(void const* argument){
	//Set up the timer and start it
	osTimerId proximitySensorTimerId = osTimerCreate(osTimer(proximitySensorTimer), osTimerPeriodic, NULL); 
	osTimerStart(proximitySensorTimerId, 20); 
	
	//These will keep track of the x and y coordinates for the measured person
	float x = -1; 
	float y = -1; 
	
	//Main Loop
	while(1){
		//Wait until the display signal is set
		osSignalWait(PROXIMITY_SENSOR_SIGNAL, osWaitForever);
		
		//Get the measured distance from the sensor
		uint8_t distance = getSensorDistance(); 
		
		printf("Distance: %d\n", distance); 
		
		//If the distance was 0, just set the x and y coordinates to -1
		if(distance == 0){
			x = -1; 
			y = -1; 
		}
		else{
			//Get the angle from the motor
			uint8_t angle = getMotorAngle(); 
			
			//From this, calculate x and y 
			//X is cos(angle) * distance, with angle converted to radians
			x = cos(angle * PI / 180.0) * distance;
			//Y is sin(angle) * distance, with angle converted to radians
			y = sin(angle * PI / 180.0) * distance;
			
		}
		
		//Get the mutex 
		osMutexWait(sensorCoordinatesId, osWaitForever); 
		
		//Set the x and y values
		sensorCoordinates.x = x; 
		sensorCoordinates.y = y; 
		
		//Release the mutex
		osMutexRelease(sensorCoordinatesId); 
		
		//Update the motor angle
		updateMotor(); 
	}
}

/**
	The wireless thread
*/
void wireless(void const* arg){
	//TODO
}

/**
	Handles the timer callback for updating the display
*/
void displayTimerCallback(void const* argument){
	//Send a signal to the lcd thread
	osSignalSet(lcd_thread, DISPLAY_SIGNAL); 
}

/**
	Handles the timer callback for retrieving values from the proximity sensor
*/
void proximitySensorTimerCallback(void const* arg){
	//Send a signal to the sensor thread
	osSignalSet(proximity_sensor_thread, PROXIMITY_SENSOR_SIGNAL); 
}
