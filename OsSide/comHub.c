#include "data_Transfer.h"
#include "cmsis_os.h"                     // RTOS object definitions                                      // CMSIS RTOS header file



osMailQDef (mail_pool_in, 10, mail_protocol_t);
osMailQDef (mail_pool_spi, 10, mail_protocol_t);
osMailQId  (mail_q_id[5]);

extern osMailQId PcalQ_Id;

extern osThreadId tid_SPI;
extern osThreadId tid_comhub;

void comhub_init()
{
	NRF_LOG_INFO("init com hub\r\n");
	// mail subsctiption system implementation
		mail_q_id[0] 			= osMailCreate(osMailQ(mail_pool_in), tid_comhub);
		mail_q_id[1] 			= osMailCreate(osMailQ(mail_pool_spi), tid_SPI);
	//mail_pool_q_id[2] = osMailCreate(osMailQ(mail_pool_q), NULL);
	//mail_pool_q_id[3] = osMailCreate(osMailQ(mail_pool_q), NULL);
}
void comhub(void const *argument)
{
	osEvent evt;
		//init malqueue then
		//wait for mail			
	NRF_LOG_INFO("ComHub start\r\n");

	while(1)
	{
		evt = osMailGet(mail_q_id[0],osWaitForever);
		if(evt.status == osEventMail)
		{
			mail_protocol_t *received = (mail_protocol_t *)evt.value.p;
			
			NRF_LOG_INFO("mail %x recieved from, %i, sending to %i\r\n" ,*received->pld ,received->rid,received->sid);

			osMailPut(mail_q_id[1], received);			
			if(!received)
			{
				NRF_LOG_INFO("There is no mail\n\r")
			}
			
			NRF_LOG_INFO("sending to mail queue %x\r\n", (uint32_t)&mail_q_id[1]);
		}
	}
}

//old code, might need it
/*
uint8_t recieveMail(mail_protocol_t *rptr, uint8_t rId)
{
	if(rptr->rid == rId)
		return 1;
	else
		osMailPut(PcalQ_Id, rptr);
		return 0;
}
uint8_t sendMail(uint8_t flags, uint8_t *package, mail_protocol_t *sptr, uint8_t sId, uint8_t rId)
{
	sptr = osMailAlloc(PcalQ_Id, osWaitForever);
	sptr->sid = sId;
	sptr->rid = rId;
	sptr->pld = package;
	sptr->flg = flags;
	osMailPut(PcalQ_Id, sptr);
}*/
