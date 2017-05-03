#include "util.h"
#include "data_Transfer.h"

extern osThreadId tid_SPI;
extern osThreadId tid_comhub;
extern osThreadId tid_brn;
extern osThreadId tid_TWI;
extern osMailQId mail_q_id[5];


void MainControlUnit(const void *arguments)
{
	osEvent evt;
	int8_t lim[1];
	while(1)
	{
		evt = osMailGet(BRN_Q,osWaitForever);
		if(evt.status == osEventMail)
		{
			mail_protocol_t *received = (mail_protocol_t *)evt.value.p;
			switch(received->pld[0])
			{
				//T for temperature commands
				case ('t'):
				case ('T'):
					switch(received->pld[1])
					{
						// TR for reading temperature
						case('r'):
						case('R'):
							
							osSignalSet(tid_TWI,1);
							break;
						// TA for setting temperature alert
						case('a'):
						case('A'):
							sscanf((char*)received->pld,"%*s %i",lim);
							send_mail(BRN_ID,TWI_ID,0x01,1,lim);
							osSignalSet(tid_TWI,2);
							break;
						default:
							break;
					}
						break;
					default:
						break;
				}
			osMailFree(BRN_Q,received);
		}
	}
}
