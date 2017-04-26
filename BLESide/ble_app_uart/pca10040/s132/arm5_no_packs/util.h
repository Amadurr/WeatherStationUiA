#ifndef __UTIL_H
#define __UTIL_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void fifo_init(void);
void spi_app_init(void);
void spi_handler(void);
void add_fifo(uint8_t *Data);
void butt_init(void);
typedef struct Data
{
  uint8_t         max_size;    
  uint8_t         **array;   		     
	uint8_t				 	head;		 		
	uint8_t					tail;
	uint8_t					used;
} fifo_list;

#endif
