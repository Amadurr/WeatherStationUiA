#include "data_Transfer.h"
#include "cmsis_os.h"                     // RTOS object definitions                                      // CMSIS RTOS header file
#include "nrf_drv_spi.h"
#include "drv_pcal.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

typedef struct{
	uint8_t sid;	// sender ID
	uint8_t rid;	// reciever ID
	uint8_t flg;  // flags
	uint32_t *pld;	// pointer to payload
}	mail_protocol_t;


osMailQDef (mail_pool_q, 10, mail_protocol_t);
osMailQId  (*mail_pool_q_id);


void comhub()
{
	// mail subsctiption system implementation
	mail_pool_q_id[0] = osMailCreate(osMailQ(mail_pool_q), NULL);
	mail_pool_q_id[1] = osMailCreate(osMailQ(mail_pool_q), NULL);
	mail_pool_q_id[2] = osMailCreate(osMailQ(mail_pool_q), NULL);
	mail_pool_q_id[3] = osMailCreate(osMailQ(mail_pool_q), NULL);
	osEvent evt;
	/*
		init malqueue then
		wait for mail
	*/
	
	while(1)
	{
		evt = osWait(osWaitForever);
		if(evt.status == osEventMail)
		{
			mail_protocol_t *received = (mail_protocol_t *)evt.value.p;
			
			osMailPut(mail_pool_q_id[received->rid], received);
			osMailFree(mail_pool_q_id[received->sid], received);
		}
	}
}