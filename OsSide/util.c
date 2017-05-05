
#include "cmsis_os.h"                     // RTOS object definitions                                      // CMSIS RTOS header file
#include <stdarg.h>
#include "data_Transfer.h"
#include "util.h"

uint8_t data[] =	"ABC";
extern uint8_t command;
#define BUTT 13

extern osThreadId tid_comhub;
extern osMailQId  (mail_q_id[5]);
/*
void butt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	//NRF_LOG_INFO("sending testmsg\r\n");
	uint8_t msg[] = {'A','B','C'}; 
	
	mail_ptc_t *testmsg;
	testmsg = (mail_protocol_t *) osMailAlloc(mail_q_id[0], osWaitForever);
	if(testmsg == NULL)
	{
		
		NRF_LOG_INFO("failed to make mail\r\n");
		NRF_LOG_FLUSH();
		
	}
	testmsg->sid = 255;
	testmsg->rid = 0;
	testmsg->flg = 0;
	testmsg->pld_s = sizeof(msg);
	testmsg->pld = msg;
	osMailPut(COM_Q, testmsg);
}

void butt_init(void)
{    
	ret_code_t err_code;

	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
	in_config.pull = NRF_GPIO_PIN_PULLUP;

	err_code = nrf_drv_gpiote_in_init(BUTT, &in_config, butt_handler);
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_event_enable(BUTT, true);

}*/


