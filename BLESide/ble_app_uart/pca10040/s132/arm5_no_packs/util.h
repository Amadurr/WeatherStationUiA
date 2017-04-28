#ifndef U_PRINT
#define U_PRINT
#define uprint(...)\
{\
	snprintf((char*)printbuf,sizeof(printbuf),__VA_ARGS__);\
	uartprint((uint8_t*)printbuf);\
}
#endif //U_PRINT

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


void uart_print_init(void);
void uartprint(uint8_t *ch);

extern uint8_t printbuf[50];

typedef struct Data
{
  uint8_t         max_size;    
  uint8_t         **array;   		     
	uint8_t				 	head;		 		
	uint8_t					tail;
	uint8_t					used;
} fifo_list;

#endif



