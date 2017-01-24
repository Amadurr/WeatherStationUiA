#include "uart_print.h"
#include "pca10040.h"

nrf_drv_uart_t uart_inst = NRF_DRV_UART_INSTANCE(0);
nrf_drv_uart_config_t uart_conf = NRF_DRV_UART_DEFAULT_CONFIG;

osMutexId mut_print;
osMutexDef(mut_print);

osMailQId (printQ_Id);
osMailQDef(printQ, 20, print_f);

ret_code_t UartPrintInit(void)
{
	
	uart_conf.pseltxd = TX_PIN_NUMBER;
	uart_conf.pselrxd = RX_PIN_NUMBER;
	uart_conf.pselcts = CTS_PIN_NUMBER;
	uart_conf.pselrts = RTS_PIN_NUMBER;
	mut_print = osMutexCreate(osMutex(mut_print));
	printQ_Id = osMailCreate(osMailQ(printQ),NULL);
	return nrf_drv_uart_init(&uart_inst, &uart_conf, NULL);

	
}

void UartPrintf(const char* fmt, ...)
{
	print_f *print_data;
	print_data = (print_f *) osMailAlloc(printQ_Id, osWaitForever);
	va_list args;
	va_start (args, fmt);
	vsnprintf(print_data->buffer,50, fmt, args);
	
	va_end (args);
	osMailPut(printQ_Id, print_data);
}

void UartPrint()
{
	ret_code_t errcode;
	osEvent evt;
	print_f *print;
	while(1)
	{
		evt = osMailGet(printQ_Id,osWaitForever);
		if(evt.status == osEventMail)
		{
			print = evt.value.p;
			osMutexWait(mut_print, osWaitForever);
			for(int i = 0; i < strlen(print->buffer); i++)
			{
				errcode = nrf_drv_uart_tx(&uart_inst, (const uint8_t *)&print->buffer[i], 1);
				if(errcode != NRF_SUCCESS)
				{
					osMutexRelease(mut_print);
					return;
				}
			}
			osMutexRelease(mut_print);
		}
		
	}
}


