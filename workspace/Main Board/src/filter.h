#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

//The depth of our filter
#define FILTER_DEPTH 20
//Random variable to designate empty spaces in the buffer
//Needed to be a variable that was not possible to obtain
#define EMPTY 10000

/** Our filter struct:
	@param index The index of the next number to replace
	@param buffer The array of all of the recorded values
	@param average The current average of the values in the bugger
*/
typedef struct {
	int index;
	float buffer[FILTER_DEPTH];
	float average;
}filter;

extern void calculateMovingAverage(filter *filter, float newValue); 
void initializeFilter(filter *filter);
