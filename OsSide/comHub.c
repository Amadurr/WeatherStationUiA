#include "data_Transfer.h"
#include "util.h"


osMailQDef (mail_pool_in, 20, mail_ptc_t);
osMailQDef (mail_pool_spi, 10, mail_ptc_t);
osMailQDef (mail_pool_brn, 10, mail_ptc_t);
osMailQDef (mail_pool_twi, 10, mail_ptc_t);
osMailQId  (mail_q_id[5]);

extern osMutexId Print_Mutex;
extern osMailQId PcalQ_Id;

extern osThreadId tid_SPI;
extern osThreadId tid_comhub;
extern osThreadId tid_brn;
extern osThreadId tid_TWI;

void comhub_init()
{
	osMutexWait(Print_Mutex,osWaitForever);
	NRF_LOG_INFO("init com hub\r\n");
	osMutexRelease(Print_Mutex);
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

	osMutexWait(Print_Mutex,osWaitForever);	
	NRF_LOG_INFO("ComHub start\r\n");
	osMutexRelease(Print_Mutex);
	while(1)
	{
		evt = osMailGet(COM_Q,osWaitForever);
		if(evt.status == osEventMail)
		{
			mail_ptc_t *received = (mail_ptc_t *)evt.value.p;
			
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("mail recieved from, %i, sending to %i\r\n" ,received->sid,received->rid);				
			osMutexRelease(Print_Mutex);
			//send_mail(received->sid,received->rid,received->flg,received->pld_s,received->pld);
			
			mail_ptc_t *send = (mail_ptc_t *) osMailAlloc(mail_q_id[received->rid], osWaitForever);
			if(send == NULL)
			{
				
				osMutexWait(Print_Mutex,osWaitForever);
				NRF_LOG_INFO("failed to make mail\r\n");
				NRF_LOG_FLUSH();				
				osMutexRelease(Print_Mutex);
				
			}
			memcpy(send,received,sizeof(mail_ptc_t));
			
			osMailPut(mail_q_id[received->rid], send);
			if(!received)
			{	
				
				osMutexWait(Print_Mutex,osWaitForever);
				NRF_LOG_INFO("There is no mail\n\r")
			osMutexRelease(Print_Mutex);
			}
			osMailFree(COM_Q,received);
			
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("sending to mail queue %x\r\n", (uint32_t)&mail_q_id[1]);
			osMutexRelease(Print_Mutex);
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
	mail_ptc_t *tmp_msg = (mail_ptc_t *) osMailAlloc(COM_Q, osWaitForever);
	if(tmp_msg == NULL)
	{
		
		osMutexWait(Print_Mutex,osWaitForever);
		NRF_LOG_INFO("failed to make mail\r\n");
		osMutexRelease(Print_Mutex);
		NRF_LOG_FLUSH();
		
	}
	tmp_msg->sid = sid;
	tmp_msg->rid = rid;
	tmp_msg->flg = flg;
	tmp_msg->pld_s = pld_s;
	tmp_msg->pld = pld;
	osMailPut(COM_Q, tmp_msg);
}
