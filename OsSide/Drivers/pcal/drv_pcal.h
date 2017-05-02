
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
	TEMP_REG  							= 0x00,
	CONF_REG       					= 0x01,
	THYST      							= 0x02,
	TOS            					= 0x03,
	TIDLE  									= 0x04,
}pct_reg_t;


typedef struct
{
	pct_reg_t reg;
	uint8_t val;
}pct_data_t;

typedef struct
{
	uint8_t addr;
	const nrf_drv_twi_t *twi;
}pct_interface_t;

void pcal_drv_init(pct_data_t*);
void pcal_drv_write(pct_data_t);
void pcal_drv_read(pct_data_t*);
void twi_test(ret_code_t);
#endif //PCAL6408_H__
