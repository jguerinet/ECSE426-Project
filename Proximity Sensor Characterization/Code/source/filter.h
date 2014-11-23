#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"


typedef struct 
{
	int i;
	uint16_t buffer[10];
	float average;
}filter;


void calculateMvgAvg(filter *filter, float newValue); 
void initializeFilter(filter *filter);
