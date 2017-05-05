#include "util.h"
#include "data_Transfer.h"
#include "saadc.h"

extern osThreadId tid_SPI;
extern osThreadId tid_comhub;
extern osThreadId tid_brn;
extern osThreadId tid_TWI;
extern osMailQId mail_q_id[5];

#define ADC_PIN 05
#define ADC_TST 06
extern uint8_t Vdc;

void MainControlUnit(const void *arguments)
{
	osEvent evt;
	int8_t lim[1];
	while(1)
	{
		evt = osMailGet(BRN_Q,osWaitForever);
		if(evt.status == osEventMail)
		{
			mail_ptc_t *received = (mail_ptc_t *)evt.value.p;
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
							
							if((received->pld[2] == 'o')||(received->pld[2] == '0'))
							{
								received->flg = 0xC0;
							}
							send_mail(BRN_ID,TWI_ID,received->flg|0x01,1,NULL);
							osMailFree(BRN_Q,received);
							
								
							break;
						// TA for setting temperature alert
						case('a'):
						case('A'):
							sscanf((char*)received->pld,"%*s %i",lim);
							send_mail(BRN_ID,TWI_ID,0x02,1,lim);
							osMailFree(BRN_Q,received);
							break;
						default:
							break;
					}
						break;
				case ('v'):
				case ('V'):
					switch(received->pld[1])
					{
						case ('r'):
						case ('R'):
							Vdc = 0;
							nrf_drv_saadc_sample();
							while(!Vdc)
							{
								osDelay(50);
							}
							send_mail(BRN_ID,SPI_ID,received->flg|0x02,1,&Vdc);
							osMailFree(BRN_Q,received);
						break;
						case ('s'):
						case ('S'):
							break;
					}
					default:
						break;
			}
		}
	}
}
