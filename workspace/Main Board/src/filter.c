/**
  ******************************************************************************
  * @file    filter.c
  * @author  Abdul Haseeb Jehangir (abdul.jehangir@mail.mcgill.ca)
             Hamza Riaz (hamza.riaz@mail.mcgill.ca)
  * @version V1.0
  * @date    09-October-2014
  * @brief   This file pimplement moving time average filter.
	*/
#include "filter.h"

/**
  * @brief  Initializes filter buffer with 0
  * @param  *f: Pointer to filter
	* @param  depth: Filter Depth D
  * @retval None
  */
void filter_init(Filter *f, int32_t* buffer_ptr, int depth){
	f->depth = depth;
	f->i = 0;
	f->sum = 0;
	
	f->addr = buffer_ptr;
}

/**
  * @brief  Inputs new data into filter buffer and removes the oldest data.
  * @param  *f: Pointer to filter
  * @param  temp: New data value
  * @retval None
  */
void filter_add (Filter *f, int32_t temp){
	f->sum -= f->addr[f->i];
	f->sum += temp;
	f->addr[f->i] = temp;
	f->i++;
	
	if (f->i == f->depth) {
		f->i = 0;
	}
}

/**
  * @brief  Calculate filter average value.
  * @param  *f: pointer to the filter
  * @retval None
  */
int32_t filter_avg(Filter *f) {
	return f->sum / f->depth;
}
