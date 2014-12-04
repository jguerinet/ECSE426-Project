#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  		// Device header

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include "filter.h"
#include "cc2500interface.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265

/* AREA SIZE (cm) */
#define MAX_X 400
#define MAX_Y	400

/* Filter Depth */
#define PROXIMITY_FILTER_DEPTH   20
#define RSSI_FILTER_DEPTH        20

/* SIGNALS */
//Display Update Signal 
#define DISPLAY_SIGNAL (int32_t) 0x01
//Proximity Sensor Signal
#define PROXIMITY_SENSOR_SIGNAL (int32_t) 0x02

/* TEST BOOLEANS (set to true the part you want to test */
int motor = 0; 
int sensor = 0; 
int lcdDisplay = 0; 

/* METHODS FROM OTHER FILES */
//LCD
extern void initializeLCD(void); 
extern void clearPosition(int16_t x, int16_t y);
extern void drawPosition(int16_t x, int16_t y); 
extern void drawGrid(void); 
//Proximity Sensor
extern void initializeProximitySensor(void); 
extern uint8_t getSensorDistance(void); 
//Motor
extern void initializeMotor(void);
extern void updateMotor(void); 
extern uint8_t getMotorAngle(void);
//Test
extern void testMotor(void);
extern void testProximitySensor(void); 
extern void testLCD(void); 

/* THREAD FUNCTIONS */
//LCD
void lcd(void const *arg);
//Proximity Sensor
void proximitySensor(void const *arg); 
//Wireless
void wireless(void const *arg); 

/* THREAD DEFINITIONS */
//LCD
osThreadDef(lcd, osPriorityLow, 1, 0);
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

int main (void) {
	//Check if we are testing something
	if(motor){
		testMotor();
	}
	else if(sensor){
		testProximitySensor(); 
	}
	else if(lcdDisplay){
		testLCD(); 
	}
	
	//Initialize CMSIS-RTOS
  osKernelInitialize ();                    
	
	/* LCD */
	initializeLCD(); 
	
   /* Wireless Init */
   initializeWireless();
   
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
	//This will keep track of the x and y coordinates that are currently mapped 
	int16_t xMapped = -1; 
	int16_t yMapped = -1;
	
	//This will keep track of the received x and y coordinates
	int16_t x = -1; 
	int16_t y = -1; 
	
	//Set up the timer and start it
	osTimerId displayTimerId = osTimerCreate(osTimer(displayTimer), osTimerPeriodic, NULL); 
	osTimerStart(displayTimerId, 300); 
	
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
			clearPosition(xMapped, yMapped); 
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
		
		//Draw the grid
		drawGrid(); 
		
		//Show the position on the screen if it is in bounds and existant
		if (xMapped >= 10 && xMapped <= 230 & yMapped >= 10 && yMapped <= 310){
			drawPosition(xMapped, yMapped);  
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
	osTimerStart(proximitySensorTimerId, 100); 
	
	//These will keep track of the x and y coordinates for the measured person
	float x = -1; 
	float y = -1; 
	
	//Moving Average filter for the distances
	Filter filter;
   uint32_t filter_buffer[PROXIMITY_FILTER_DEPTH];
	filter_init(&filter, (int32_t*)filter_buffer, PROXIMITY_FILTER_DEPTH); 
	
	//Main Loop
	while(1){
		//Wait until the display signal is set
		osSignalWait(PROXIMITY_SENSOR_SIGNAL, osWaitForever);
		
		//Get the measured distance from the sensor
		filter_add(&filter, getSensorDistance());
		uint8_t distance = (uint8_t)filter_avg(&filter); 
		
		//printf("Distance: %d\n", distance); 
		
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
	uint8_t last_known_seq, user_rssi_dec;
	Packet pkt;
   float rssi_user, rssi_aux;
   uint8_t filter_aux_buffer[RSSI_FILTER_DEPTH], filter_user_buffer[RSSI_FILTER_DEPTH];
   Filter filter_aux, filter_user;
   
   // Initialize filters
   filter_init(&filter_aux, (int32_t*)filter_aux_buffer, RSSI_FILTER_DEPTH);
   filter_init(&filter_user, (int32_t*)filter_user_buffer, RSSI_FILTER_DEPTH);
   
	// define the aux filter and user filter	
	printf("Thread_started. waitig for signal\n");
	//Put the board in Rx mode
	CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
	
	while(1){
		osSignalWait(RX_PKT, osWaitForever);
		GPIO_ToggleBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_14);
		CC2500_Read((uint8_t*)&pkt, CC2500_FIFO_ADDR, SMARTRF_SETTING_PKTLEN + 2);
    
		// if the packet recieved is from user, store the seq and rssi
		if (pkt.Src_addr == 0x01) {
			last_known_seq = pkt.Seq_num;
			user_rssi_dec = pkt.Rssi;
		}
		
		// if the packet recieved is from aux board, and seq number match, we have
		// a complete set of readings
		if (pkt.Src_addr == 0x02 && pkt.Seq_num == last_known_seq) {
			rssi_aux = CC2500_ComputeRssi((float)pkt.Aux_rssi);
			
			printf("SEQ: %u\t\tRSSI_USER: %0.02f\t\tRSSI_AUX: %0.02f\n", last_known_seq, rssi_user, rssi_aux);
			// pass through the filter
         filter_add(&filter_aux, user_rssi_dec);
         filter_add(&filter_user, pkt.Aux_rssi);
			// get the average frmo the filters
			rssi_user = CC2500_ComputeRssi((float)filter_avg(&filter_user));
         rssi_aux = CC2500_ComputeRssi((float)filter_avg(&filter_aux));
			// go through database to convert rssi  -> x,y
		}
    CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
	}
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

/* Interrupt service routine on packet recieve */
void EXTI4_IRQHandler(void) {
	if(EXTI_GetITStatus(CC2500_SPI_INT_EXTI_LINE) != RESET)
    {
			osSignalSet(wireless_thread, RX_PKT);
		}	
	// clear EXTI inruppt pending bit.
	EXTI_ClearITPendingBit(CC2500_SPI_INT_EXTI_LINE);
}
