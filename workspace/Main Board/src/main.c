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

/* SIGNALS */
//Display Update Signal 
#define DISPLAY_SIGNAL (int32_t) 0x01

/* THREAD FUNCTIONS */
//LCD
void lcd(void const *argument);
//Proximity Sensor
void proximitySensor(void const *argument); 

/* THREAD DEFINITIONS */
//LCD
osThreadDef(lcd, osPriorityNormal, 1, 0);
//Proximity Sensor
osThreadDef(proximitySensor, osPriorityNormal, 1, 0); 

/* THREAD IDs */
//LCD
osThreadId lcd_thread;
//Proximity Sensor
osThreadId proximity_sensor_thread; 

/* MUTEX DEFINTIIONS */
//Position measured by the sensor
osMutexDef(sensorCoordinates);

/* MUTEX IDs */
osMutexId sensorCoordinatesId;

//This for the timer to update the display 
void displayTimerCallback(void const* arg);
osTimerDef(displayTimer, displayTimerCallback); 

//This struct will contain x and y coordinates measured
typedef struct{
	float x; 
	float y;
} Coordinates; 

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
	
	//Set up the Coordinates struct for the motion sensor 
	//Initially everything is -1
	Coordinates sensorCoordinates = {-1};
	
	/* Mutex Creations */
	sensorCoordinatesId = osMutexCreate(osMutex(sensorCoordinates)); 
	
	/* Thread Creations */
	lcd_thread = osThreadCreate(osThread(lcd), &sensorCoordinates);
	proximity_sensor_thread = osThreadCreate(osThread(proximitySensor), &sensorCoordinates); 
	
	//Start thread execution
	osKernelStart();                         
}

/**
	* The LCD screen thread. 
  */
void lcd(void const *argument){
	//Get the sensor coordinates from the arguments
	Coordinates *sensorCoordinates = (Coordinates *)argument; 
	
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
	double xMapped = -1; 
	double yMapped = -1;
	
	//Set up the timer and start it
	osTimerId displayTimerId = osTimerCreate(osTimer(displayTimer), osTimerPeriodic, NULL); 
	osTimerStart(displayTimerId, 5); 
	
	//Main Loop
	while(1){
		//Wait until the display signal is set
		osSignalWait(DISPLAY_SIGNAL, osWaitForever);
		
		//Get the sensor coordinates mutex
		osMutexWait(sensorCoordinatesId, osWaitForever); 
		
		//Get the coordinates from this
		double x = sensorCoordinates->x;
		double y = sensorCoordinates->y;
		
		//Release the sensor coordinates mutex
		osMutexRelease(sensorCoordinatesId); 
		
		//If there was a previous position, hide it
		if(xMapped != -1){
			LCD_SetTextColor(LCD_COLOR_WHITE);
			LCD_DrawFullCircle(xMapped, yMapped, 4);
		}
		
		//Calculate the new x and y positions to map
		xMapped = (x+(10/44))*44;
		yMapped = (y+(10/60))*60;
		
		//Show the position on the screen if it is in bounds
		if (xMapped < 220 & yMapped < 300){
			LCD_SetTextColor(LCD_COLOR_RED);
			LCD_DrawFullCircle(xMapped, yMapped, 4);
		}
		else{
			//Printf error message
			printf("Out of Bounds: %f, %f", xMapped, yMapped);
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
	//Get the sensor coordinates from the arguments
	Coordinates *sensorCoordinates = (Coordinates *)argument; 
}

/**
	Handles the timer callback for updating the display
*/
void displayTimerCallback(void const* argument){
	//Send a signal to the lcd thread
	osSignalSet(lcd_thread, DISPLAY_SIGNAL); 
}
