#include "util.h"
#include "data_Transfer.h"
extern osMailQId mail_q_id[5];

extern osThreadId tid_SPI;
extern osThreadId tid_comhub;
extern osThreadId tid_brn;
extern osThreadId tid_TWI;


void MainControlUnit(const void *arguments)
{
	osEvent evt;
	while(1)
	{
		evt = osMailGet(mail_q_id[BRN_ID],osWaitForever);
		if(evt.status == osEventMail)
		{
			mail_protocol_t *received = (mail_protocol_t *)evt.value.p;
			if((received->pld[0] == 't')||(received->pld[0] =='T'))
			{
				osSignalSet(tid_TWI,1);
			}

		}
	}
}
