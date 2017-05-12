
#ifndef UTIL_H__
#define UTIL_H__
#include "cmsis_os.h"                     // RTOS object definitions                                      // CMSIS RTOS header file
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nordic_common.h"
#include "bsp.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_drv_gpiote.h"
#include "string.h"

#include "nrf_peripherals.h"
#include "nrf_drv_twi.h"


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
	uint8_t val[2];
}pct_data_t;

typedef struct
{
	uint8_t addr;
	const nrf_drv_twi_t *twi;
}pct_interface_t;

void pct_drv_init(pct_data_t*);
void pct_drv_write(pct_data_t);
void pct_drv_read(pct_data_t*);
void twi_test(ret_code_t);


void butt_init(void);
void twi_init(void);
void twi(void);
void TWI_controller(const void *arguments);

void MainControlUnit(const void *arguments);

void send_mail
(
		uint8_t sid,
		uint8_t rid,
		uint8_t flg,
		uint8_t pld_s,
		uint8_t *pld
);

typedef enum
{
	COM_ID    = 0x00,
	SPI_ID		= 0x01,
	BRN_ID		= 0x02,
	TWI_ID		= 0x03,
	DAC_ID		= 0x04
}mail_id_t;
/*
mail IDs
spi controll 	: 0x01
brain 				: 0x02
twi						:	0x03


*/


#endif //UTIL_H__
