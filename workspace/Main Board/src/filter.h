/**
  ******************************************************************************
  * @file    filter.h
  * @author  Abdul Haseeb Jehangir (abdulDOTjehangir@mail.mcgill.ca)
             Hamza Riaz (hamzaDOTriaz@mail.mcgill.ca)
  * @version V1.0
  * @date    09-October-2014
  * @brief   This file provides interface to moving time average filter.
	*/

#ifndef FILTER_H_
#define FILTER_H_
#include "stm32f4xx.h" 

/** 
  * @brief   Filter structure definition  
  */ 
typedef struct filter {
	int depth;				/*!< Specifies filter depth D. */ 
	int i;
	int sum;					/*!< Specifies sum of filter buffer */ 
	int32_t *addr;		/*!< Specifies address to filter buffer */ 
} Filter;

void filter_init(Filter *f, int32_t* buffer_ptr, int depth);
void filter_add (Filter *f, int32_t temp);
extern int32_t filter_avg (Filter *f);

#endif 
