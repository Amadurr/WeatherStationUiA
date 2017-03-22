#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "bsp.h"
#include "app_error.h"
#include "definitions.h"

#define SPIS_INSTANCE 1 /**< SPIS instance index. */
static const nrf_drv_spis_t spis = NRF_DRV_SPIS_INSTANCE(SPIS_INSTANCE);/**< SPIS instance. */
const nrf_drv_timer_t TIMER = NRF_DRV_TIMER_INSTANCE(0);
volatile uint8_t timer_;

#define TEST_STRING "AB"
static uint8_t       m_tx_buf[] = TEST_STRING;           	/**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(TEST_STRING) + 1];   /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        	/**< Transfer length. */

static volatile bool spis_xfer_done; /**< Flag used to indicate that SPIS instance completed the transfer. */

//Handler for SPI evenets
void spis_event_handler(nrf_drv_spis_event_t event)
{
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE)
    {
        spis_xfer_done = true;
        NRF_LOG_INFO(" Transfer completed. Received: %s\r\n",(uint32_t)m_rx_buf);
    }
} 

//Handler for timer events.
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
		timer_ = 1;
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            timer_ = 0;
            break;

        default:
            //Do nothing.
            break;
    }
}

void spi_app_init(void)
{
		NRF_LOG_INFO("SPIS example\r\n");

    nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG;
    spis_config.csn_pin               = APP_SPIS_CS_PIN;
    spis_config.miso_pin              = APP_SPIS_MISO_PIN;
    spis_config.mosi_pin              = APP_SPIS_MOSI_PIN;
    spis_config.sck_pin               = APP_SPIS_SCK_PIN;

    APP_ERROR_CHECK(nrf_drv_spis_init(&spis, &spis_config, spis_event_handler));
	
	    uint32_t time_ms = 50; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;
}

void spi_handler(void)
{
	while(1)
	{
		int syn = 0;
		int ack;
		
		//set timer to some time t
		nrf_drv_timer_enable(&TIMER);
		while((ack != 0 )|(timer!=0))
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

//Function for main application entry

int main(void)
{
    uint32_t time_ms = 50; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;

	
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIMER, &timer_cfg, timer_led_event_handler);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER, time_ms);

    nrf_drv_timer_extended_compare(
         &TIMER, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_STOP_MASK, true);

    nrf_drv_timer_enable(&TIMER);

    while (1)
    {
        __WFI();
    }
}
