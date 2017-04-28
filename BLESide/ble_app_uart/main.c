#include "definitions.h"
#include "BLEFunctions.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include "nrf_delay.h"



int main(void)
{

	//NRF_LOG_INFO("Log initialized\r\n");
	//NRF_LOG_FLUSH();
	uart_print_init();
	uprint("uprint initialized\r\n");
	spi_app_init();
	uprint("spi initialized\r\n");
	
	fifo_init();
	uprint("fifo initialized\r\n");
	butt_init();
	uprint("butt initialized\r\n")
	
	ble_app_init();
	uprint("BLE initialized\r\n");
	spi_handler();
	while(1){}

		
	
    // Enter main loop.
 /*   while (1)
    {
        power_manage();
    }  */  
		
}




/**
 * @}
 */
