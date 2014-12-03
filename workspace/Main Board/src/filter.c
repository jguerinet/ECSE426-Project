#include "filter.h"

/** Initialize the filter struct to default values. Buffer should be full of the EMPTY value. 
	@param filter The filter struct to initialize
*/
void initializeFilter(filter *filter) {
	//Index starts at 0
	filter->index = 0;
	
	//Set buffer values to all 0's
	for(int i = 0; i < FILTER_DEPTH; i++) {
		filter->buffer[i] = EMPTY;
	}
	
	//Average starts at 0
	filter->average = 0;
}

/** Calculate the moving average of a stream of inputs. This method is called repeatedly and operates on the filter struct handed into it.
	@param filter The filter 
	@param newValue The new value to put in the filter
*/
void calculateMovingAverage(filter *filter, float newValue) {
	//This will store the current average of the values in the filter
	float average = 0; 
	//This will store the number of current non-zero elements in the bugger
	float size = 0;
	
	//Get the current index from the filter
	int currentIndex = filter->index;
	
	//Put the newValue at the current index position iin the buffer
	filter->buffer[currentIndex] = newValue;
	
	//Find the sum of the buffer's contents if the value is not empty
	//	while keeping track of how many non-empty entries there are in the buffer 
	for(int j = 0; j < FILTER_DEPTH; j++) {
		if(filter->buffer[j] != EMPTY) {
			average += filter->buffer[j];
			size++;
		}
	}
	
	//Find the average
	average = average/size;
	
	//Store the average in the filter struct
	filter->average = average;
	
	//Increment i while watching out for the wraparound
	currentIndex ++;
	if(currentIndex == FILTER_DEPTH){
		currentIndex = 0;
	}
	
	//Store the index back into the filter struct
	filter->index = currentIndex;
}
