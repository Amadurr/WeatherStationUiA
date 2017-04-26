#include "definitions.h"
#include "BLEFunctions.h"
#include "util.h"
int main(void)
{
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

	//NRF_LOG_INFO("Log initialized\r\n");
	//NRF_LOG_FLUSH();
	//ble_app_init();
	spi_app_init();
	fifo_init();
	butt_init();
	spi_handler();
		

		
	
    // Enter main loop.
 /*   while (1)
    {
        power_manage();
    }  */  
		
}




/**
 * @}
 */
