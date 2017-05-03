#include "data_Transfer.h"
#include "util.h"


osMailQDef (mail_pool_in, 20, mail_protocol_t);
osMailQDef (mail_pool_spi, 10, mail_protocol_t);
osMailQDef (mail_pool_brn, 10, mail_protocol_t);
osMailQDef (mail_pool_twi, 10, mail_protocol_t);
osMailQId  (mail_q_id[5]);


extern osMailQId PcalQ_Id;

extern osThreadId tid_SPI;
extern osThreadId tid_comhub;
extern osThreadId tid_brn;
extern osThreadId tid_TWI;

void comhub_init()
{
	NRF_LOG_INFO("init com hub\r\n");
	// mail subsctiption system implementation
		mail_q_id[0] 			= osMailCreate(osMailQ(mail_pool_in), tid_comhub);
		mail_q_id[1] 			= osMailCreate(osMailQ(mail_pool_spi), tid_SPI);
		mail_q_id[2] 			= osMailCreate(osMailQ(mail_pool_brn), tid_brn);
	  mail_q_id[3] 			= osMailCreate(osMailQ(mail_pool_twi), tid_TWI);
}
void comhub(void const *argument)
{
	osEvent evt;
		//init malqueue then
		//wait for mail			
	NRF_LOG_INFO("ComHub start\r\n");

	while(1)
	{
		evt = osMailGet(COM_Q,osWaitForever);
		if(evt.status == osEventMail)
		{
			mail_protocol_t *received = (mail_protocol_t *)evt.value.p;
			
			NRF_LOG_INFO("mail %s recieved from, %i, sending to %i\r\n" ,*received->pld ,received->rid,received->sid);

			osMailPut(mail_q_id[received->rid], received);			
			if(!received)
			{
				NRF_LOG_INFO("There is no mail\n\r")
			}
			osMailFree(COM_Q,received);
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

void send_mail(uint8_t sid, uint8_t rid, uint8_t flg, uint8_t pld_s, uint8_t *pld)
{
	mail_protocol_t *testmsg;
	testmsg = (mail_protocol_t *) osMailAlloc(COM_Q, osWaitForever);
	if(testmsg == NULL)
	{
		
		NRF_LOG_INFO("failed to make mail\r\n");
		NRF_LOG_FLUSH();
		
	}
	testmsg->sid = sid;
	testmsg->rid = rid;
	testmsg->flg = flg;
	testmsg->pld_s = pld_s;
	testmsg->pld = pld;
	osMailPut(COM_Q, testmsg);
}
