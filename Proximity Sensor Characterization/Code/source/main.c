#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"



static volatile uint8_t ticks;

int initializeProximitySensor(void);
void initializeADC1(void);
uint16_t measureProximity(void);

/*
extern void initializeMotor(void);
extern void initializeLEDs(void);  
extern uint16_t measureTemperature(void);
extern float getTempFromVoltage(float t);
extern void updateMotor(float temperature);
extern void switchLED(int on);
*/

int main(){ 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//Initialize the ticks to 0
	ticks = 0;
	
	//Set up the ADC to read from the temperature sensor
	initializeProximitySensor();

	
	//Initialize the SysTick timer with 20ms (50Hz) period
	if(SysTick_Config(20*SystemCoreClock/1000) == 0) {
		while(1){
			//Wait for an interrupt	
			if(ticks){
				//Reset ticks flag
				ticks = 0;
				
				uint16_t voltage = measureProximity();
		
				printf("%d\n", voltage);
				
			}
		}
	}
	else {
		//SysTick failure
		printf("Failed to configure SysTick");
	}
	
	return 0;
}


int initializeProximitySensor() {
	
	//Initializes ADC1
	initializeADC1();
	
	//Enables the ADC
	ADC_Cmd(ADC1, ENABLE);
	
	//Select the ADC_Channel_1 and set sample time
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_480Cycles);
	
	
	//Initialize GPIO pin PA1
	
	//Define the initializer struct 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Initial settings
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	//We need to set them to use the analog function
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

	//we want pin A1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	
	//Initialize the LEDs
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//We need to change the mappings of the pins to the AF as well
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4); 
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4); 
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4); 
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4); 
	
	return 0;
	
}

/* Initializes the ADC1 */
void initializeADC1(void) {
	//Declare the initializer structs for the ADC 
	ADC_InitTypeDef adc_init_structure;
	ADC_CommonInitTypeDef adc_common_init_structure;
	
	//Enable the ADC bus
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//Sets the default values for the ADC
	ADC_CommonStructInit(&adc_common_init_structure);
	
	//Initialize
	ADC_CommonInit(&adc_common_init_structure);
	ADC_StructInit(&adc_init_structure);
	ADC_Init(ADC1,&adc_init_structure);
}


uint16_t measureProximity(void){	
	//Begin measuring value
	ADC_SoftwareStartConv(ADC1);
	
	//Wait until the conversion is finished
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	
	//Reset flag
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	//Get the value from the conversion
	uint16_t value = ADC_GetConversionValue(ADC1); 
	
	//Return the result
	return value;	
}

void SysTick_Handler() {
	//Set ticks flag
	ticks = 1;
}


