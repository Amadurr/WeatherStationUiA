
#ifndef DATA_TRANSFER_H__
#define DATA_TRANSFER_H__
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

#include "nrf_drv_spi.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define COM_Q  mail_q_id[0]
#define	SPI_Q  mail_q_id[1]
#define	BRN_Q  mail_q_id[2]
#define	TWI_Q  mail_q_id[3]

typedef struct{
	uint8_t sid;	// sender ID
	uint8_t rid;	// reciever ID
	uint8_t flg;  // flags
	uint8_t pld_s;// size of payload array
	uint8_t *pld;	// pointer to payload
}	mail_protocol_t;


//init functions
void SPI_init(void);
void comhub_init(void);
//thread functions
void SPI_controller(void const *argument);
void comhub(void const *argument);
#endif //DATA_TRANSFER_H__
