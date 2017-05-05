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

typedef struct 
{
	uint8_t				flgs;	
	uint8_t				data[6];
	//0x80 -> from/to ble, 0x40 -> from/to uart;
}fifo_data_t;

typedef struct
{
  uint8_t         max_size;    
  fifo_data_t     *array;   		     
	uint8_t				 	head;		 		
	uint8_t					tail;
	uint8_t					used;
} fifo_list_t;

typedef	struct
{
	uint8_t flgs;
	uint8_t pld[6];
}spi_ptc_t;

void fifo_init(void);
void spi_app_init(void);
void spi_handler(void);
void add_fifo(fifo_data_t *buf);
void read_fifo(fifo_data_t *buf);
void butt_init(void);
void spi_eval(spi_ptc_t msg);


void uart_print_init(void);
void uartprint(uint8_t *ch);

extern uint8_t printbuf[50];

#endif



