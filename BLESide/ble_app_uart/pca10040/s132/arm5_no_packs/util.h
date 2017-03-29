#ifndef __UTIL_H
#define __UTIL_H

#include "util.c"

typedef struct Data
{
	
  uint16_t         *d_ptr;       // Data pointer       
  uint16_t         *next_ptr;    // Pointer of next data     
	uint16_t				 *prev_ptr;		 // Pointer of previous data
} dataqueue;

#endif