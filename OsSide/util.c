
#include "cmsis_os.h"                     // RTOS object definitions                                      // CMSIS RTOS header file
#include <stdarg.h>
#include "data_Transfer.h"
#include "util.h"
#include "nrf_drv_uart.h"

uint8_t data[] =	"ABC";
extern uint8_t command;
#define BUTT 13
extern osThreadId tid_comhub;
extern osMailQId  (mail_q_id[5]);

static nrf_drv_uart_t p_uart = NRF_DRV_UART_INSTANCE(0);
static nrf_drv_uart_config_t p_uart_config = NRF_DRV_UART_DEFAULT_CONFIG;





	
osMailQDef (Print_Mail_Q, 10, mail_print_t);
osMailQId  (Print_Q_id);

void butt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	//NRF_LOG_INFO("sending testmsg\r\n");
	uint8_t msg[] = {'A','B','C'}; 
	static uint8_t ch[] = "AB";
	nrf_drv_uart_tx(&p_uart, (const uint8_t *)ch, sizeof(ch));
	/*
	mail_protocol_t *testmsg;
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
	osMailPut(mail_q_id[0], testmsg);*/
}

void butt_init(void)
{    
	ret_code_t err_code;

	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
	in_config.pull = NRF_GPIO_PIN_PULLUP;

	err_code = nrf_drv_gpiote_in_init(BUTT, &in_config, butt_handler);
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_event_enable(BUTT, true);

}

void printer(const char *fmt,...)
{
	uint8_t* buff
	va_list args;
	va_start(args,fmt)
	//build string
	va_end(args)
	mail_print_t *printmsg;
	printmsg = (mail_print_t *) osMailAlloc(Print_Q_id, osWaitForever);
	if(printmsg == NULL)
	{
		NRF_LOG_INFO("failed to make mail\r\n");
		NRF_LOG_FLUSH();
	}
	printmsg->str = buff;
	osMailPut(Print_Q_id, printmsg);
}
void uart_print_init(void)
{
			                           
		p_uart_config.pseltxd            = TX_PIN_NUMBER;                           \
		p_uart_config.pselrxd            = RX_PIN_NUMBER;                            \
		p_uart_config.pselcts            = CTS_PIN_NUMBER;                            \
		p_uart_config.pselrts            = RTS_PIN_NUMBER;   
	
    nrf_drv_uart_init(&p_uart, &p_uart_config, NULL);
}

void print_server(void const *arg)
{
	//init stuff
	osEvent evt;
	while(1) //thread loop, do not break
	{
		evt = osMailGet(Print_Q_id,osWaitForever);
		if(evt.status == osEventMail)
		{
			mail_print_t *received = (mail_print_t *)evt.value.p;		
			nrf_drv_uart_tx(&p_uart, (const uint8_t *)received->str, strlen((char *)received->str));
		}
	}
}