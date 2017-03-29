#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <stdint.h>

void spi_app_init(void);

typedef struct fifo_list
{
  uint8_t       *array;    
  uint8_t       head;			     
	uint8_t				tail;
	uint8_t				max_size;
	uint8_t				used;
} fifo_list;

#endif