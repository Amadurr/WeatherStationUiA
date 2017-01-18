
#ifndef PCAL6408_H__
#define PCAL6408_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "nrf_peripherals.h"
#include "nrf.h"
#include "nrf_drv_twi.h"
#include "app_error.h"


typedef enum
{
	INPUT_PORT  							= 0x00,
	OUTPUT_PORT       				= 0x01,
	POLARITY_INVERSION      	= 0x02,
	CONFIGURATION            	= 0x03,
	OUTPUT_DRIVE_STRENGHT_0  	= 0x40,
	OUTPUT_DRIVE_STRENGHT_1  	= 0x41,
	INPUT_LATCH 							= 0x42,
	PULL_UP_PULL_DOWN_ENABLE 	= 0x43,
	PULL_UP_PULL_DOWN_SELECT 	= 0x44,
	INTERRUPT_MASK 						= 0x45,
	INTERRUPT_STATUS 					= 0x46,
	OUTPUT_PORT_CONFIGURATION = 0x4f
}pcal_reg_t;

typedef enum
{
	LED1 = 0x10,
	LED2 = 0x20,
	LED3 = 0x40,
	LED4 = 0x80
}pcal_led;
typedef struct
{
	pcal_reg_t reg;
	uint8_t val;
}pcal_data_t;

typedef struct
{
	uint8_t addr;
	const nrf_drv_twi_t *twi;
}pcal_interface_t;

void pcal_drv_init(pcal_data_t*);
void pcal_drv_write(pcal_data_t);
void pcal_drv_read(pcal_data_t*);
void pcal_drv_led_on(uint8_t);
void pcal_drv_led_off(uint8_t);
void twi_test(ret_code_t);
#endif //PCAL6408_H__
