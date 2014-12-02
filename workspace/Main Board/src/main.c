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
//Proximity Sensor Signal
#define PROXIMITY_SENSOR_SIGNAL (int32_t) 0x02

/* METHODS FROM OTHER FILES */
extern void initializeProximitySensor(void); 
extern uint16_t measureProximity(void); 
extern void initializeMotor(void); 

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
osThreadDef(proximitySensor, osPriorityNormal, 1, 0); 
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

//This struct will contain addresses to other structs (to pass multiple structs to the LCD thread
typedef struct{
	Coordinates *sensorCoordinates; 
	Coordinates *wirelessCoordinates; 
} StructAddresses;

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
	
	/* PROXIMITY SENSOR */
	initializeProximitySensor(); 
	
	/* MOTOR */
	initializeMotor(); 
	
	//Set up the Coordinates struct for the motion sensor 
	//Initially everything is -1
	Coordinates sensorCoordinates = {-1};
	
	//Set up the Coordinates struct for the wireless setuo 
	//Initially everything is -1
	Coordinates wirelessCoordinates = {-1};
	
	//Bundle these coordinates into 1 for the LCD thread
	StructAddresses addresses = {}; 
	addresses.sensorCoordinates = &sensorCoordinates; 
	addresses.wirelessCoordinates = &wirelessCoordinates;
	
	/* Mutex Creations */
	sensorCoordinatesId = osMutexCreate(osMutex(sensorCoordinates));
	wirelessCoordinatesId = osMutexCreate(osMutex(wirelessCoordinates));
	
	/* Thread Creations */
	lcd_thread = osThreadCreate(osThread(lcd), &addresses);
	proximity_sensor_thread = osThreadCreate(osThread(proximitySensor), &sensorCoordinates); 
	wireless_thread = osThreadCreate(osThread(wireless), &wirelessCoordinates); 
	
	//Start thread execution
	osKernelStart();                         
}

/**
	* The LCD screen thread. 
  */
void lcd(void const *arg){
	//Get the addresses for the coordinates struct from the arg
	StructAddresses *addresses = (StructAddresses *)arg; 
	
	//Get the coordinates structs from this
	Coordinates *sensorCoordinates = addresses->sensorCoordinates;
	Coordinates *wirelessCoordinates = addresses->wirelessCoordinates;
	
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

	//Set up the timer and start it
	osTimerId proximitySensorTimerId = osTimerCreate(osTimer(proximitySensorTimer), osTimerPeriodic, NULL); 
	osTimerStart(proximitySensorTimerId, 10); 

	//Start the motor timer 
	TIM_Cmd(TIM4, ENABLE); 
	
	//Main Loop
	while(1){
		//Wait until the display signal is set
		osSignalWait(PROXIMITY_SENSOR_SIGNAL, osWaitForever);
		
		//Get the measured distance from the sensor
		uint16_t distance = measureProximity(); 
	}
}

/**
	The wireless thread
*/
void wireless(void const* arg){
	//Get the wireless coordinates from the arguments
	Coordinates *wirelessCoordinates = (Coordinates *)arg;
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
