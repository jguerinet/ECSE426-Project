#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "temperatureSensor.h"


void initializeADC1(void);

/* Initializes the temperature sensor */
void initializeTemperatureSensor(void) {
	//Initializes ADC1
	initializeADC1();
	
	//Enables the ADC
	ADC_Cmd(ADC1, ENABLE);
	
	//Enable the internal connections of the temperature sensor and Vrefint sources with the ADC channels
	ADC_TempSensorVrefintCmd(ENABLE);
	
	//Select the ADC_Channel_TempSensor and set sample time
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles);
		
}	

/* Measures the temperature from the ADC. 
	Returns a value in volts */
uint16_t measureTemperature(void){	
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

/* Returns the temperature from a given voltage */
float getTempFromVoltage(float v) {	
	//Get celsius
	return (((v * 3/4095)  - 0.76) / 0.0025) + 25;
}
