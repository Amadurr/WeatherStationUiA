#include "definitions.h"
#include "BLEFunctions.h"

#define SPIS_INSTANCE 1 /**< SPIS instance index. */
static const nrf_drv_spis_t spis = NRF_DRV_SPIS_INSTANCE(SPIS_INSTANCE);/**< SPIS instance. */

#define TEST_STRING "AB"
static uint8_t       m_tx_buf[] = TEST_STRING;           /**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(TEST_STRING) + 1];    /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */

static volatile bool spis_xfer_done; /**< Flag used to indicate that SPIS instance completed the transfer. */ 
extern volatile uint8_t timer_;

/**
 * @brief SPIS user event handler.
 *
 * @param event
 */
void spis_event_handler(nrf_drv_spis_event_t event)
{
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE)
    {
        spis_xfer_done = true;
        NRF_LOG_INFO(" Transfer completed. Received: %s\r\n",(uint32_t)m_rx_buf);
    }
} 


// @brief Application main function.
 
void spi_app_init(void)
{
		NRF_LOG_INFO("SPIS example\r\n");

    nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG;
    spis_config.csn_pin               = APP_SPIS_CS_PIN;
    spis_config.miso_pin              = APP_SPIS_MISO_PIN;
    spis_config.mosi_pin              = APP_SPIS_MOSI_PIN;
    spis_config.sck_pin               = APP_SPIS_SCK_PIN;

    APP_ERROR_CHECK(nrf_drv_spis_init(&spis, &spis_config, spis_event_handler));
		
}

void spi_handler(void)
{
	while(1)
	{
		int syn = 0;
		int ack;
		
		//set timer to some time t
		ti
		while((ack != 0 )|( timer!=0))
		{
			__WFE();
		}
		
		if(ack)
		{
			//prepare transfer
			//dele pakkene
			// sette opp register
			//gjøre klar buffer
			spis_xfer_done = false;
			
			syn = 0;
			while(!spis_xfer_done)
			{
				__WFE();
			}
			return;
		}
		else
		{
			while(/*!spi_transfer done*/1)
			{
				__WFE();
			}
		}
	}
}

int main(void)
{
		ble_app_init();
		spi_app_init();

     while (1)
    {
        memset(m_rx_buf, 0, m_length);
        spis_xfer_done = false;

        APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length, m_rx_buf, m_length));

        while (!spis_xfer_done)
        {
            __WFE();
        }

        NRF_LOG_FLUSH();

        bsp_board_led_invert(BSP_BOARD_LED_0);
			
    }	
		
	
    // Enter main loop.
 /*   while (1)
    {
        power_manage();
    }  */  
		
}




/**
 * @}
 */
