
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
#define	DAC_Q  mail_q_id[4]

typedef struct
{
	uint8_t sid;	// sender ID
	uint8_t rid;	// reciever ID
	uint8_t pld_s;// size of payload array
	uint8_t flg;  // flags
	uint8_t *pld;	// pointer to payload
}	mail_ptc_t;		//mail protocol type
typedef struct
{
	uint8_t flgs;
	uint8_t pld[6];
	/* flags
	0x80 = to ble
	0x40 = to uart
	0x01 = temp data
	0x02 = temp_thresh
	
	*/
}spi_ptc_t;			//spi protocol type

//init functions
void SPI_init(void);
void comhub_init(void);
//thread functions
void SPI_controller(void const *argument);
void comhub(void const *argument);
void DAC_controller(void const *argument);

void start_spi_transfer(nrf_drv_spi_t const * const p_instance,
																		uint8_t 				p_CS,
																		uint8_t const * p_tx_buffer,
																		uint8_t         tx_buffer_length,
																		uint8_t       * p_rx_buffer,
																		uint8_t         rx_buffer_length);
#endif //DATA_TRANSFER_H__
