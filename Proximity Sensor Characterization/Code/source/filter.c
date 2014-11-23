#include "filter.h"
uint8_t filterDepth = 10;

/* Initialize the filter struct to default values. Buffer should be full of zeroes. */
void initializeFilter(filter *filter) {
	//Index starts at 0
	filter->i = 0;
	
	//Set buffer to all 0's
	for(int i = 0; i<filterDepth; i++) {
		filter->buffer[i] = 0.0;
	}
	
	//Average starts at 0
	filter->average = 0;
}

/* Calculate the moving average of a stream of inputs. This method is called repeatedly and operates on the filter struct handed into it.*/
void calculateMvgAvg(filter *filter, float newValue) {
	float average = 0;
	float size = 0;
	
	//Get the current index from the filter
	int i = filter->i;
	
	//Reset i to 0 when it reaches the end of the buffer
	if(i == filterDepth){
		i=0;
	}
	
	//Put the newValue in the last position in the buffer
	filter->buffer[i] = newValue;
	
	//Find the sum of the buffer's contents if the item is not zero while keeping track of how many non-zero entries there are in the buffer 
	for(int j =0; j<filterDepth; j++) {
		if(filter->buffer[j] != 0) {
			average += filter->buffer[j];
			size++;
		}
	}
	
	//Find the average
	average = average/size;
	
	//Store the average in the filter struct
	filter->average = average;
	
	//Increment the iterator, store it in the filter struct
	filter->i = ++i;
}
