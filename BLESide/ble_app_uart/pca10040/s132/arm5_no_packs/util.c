#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "nrf_delay.h"
#include "bsp.h"
#include "app_error.h"
#include "definitions.h"
#include "util.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"


#define SPIS_INSTANCE 1 /**< SPIS instance index. */
static const nrf_drv_spis_t spis = NRF_DRV_SPIS_INSTANCE(SPIS_INSTANCE);/**< SPIS instance. */
const nrf_drv_timer_t TIMER = NRF_DRV_TIMER_INSTANCE(0);
volatile uint8_t timer_;
uint8_t* read_fifo(void);

#define TEST_STRING "ABC"
static uint8_t       m_tx_buf[] = TEST_STRING;           	/**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(TEST_STRING) + 1];   /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        	/**< Transfer length. */
static volatile bool spis_xfer_done; /**< Flag used to indicate that SPIS instance completed the transfer. */
fifo_list d_list; 									 /**< Legger til struct fifo_list. */
uint8_t command;
uint8_t syn = 0x00;
#define PIN_SYN 31
#define PIN_ACK 30

//Handler for SPI evenets
void spis_event_handler(nrf_drv_spis_event_t event)
{
	
		NRF_LOG_INFO("transfering\r\n");
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE)
    {
        spis_xfer_done = true;
        NRF_LOG_INFO(" Transfer completed. Received: %s\r\n",(uint32_t)m_rx_buf);
    }
} 

//Handler for timer events.
void timer_led_event_handler(nrf_timer_event_t event_type, void *p_context)
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

void SynHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
    {
			syn ^= 0x01;
}
		
void spi_app_init(void)
{
		NRF_LOG_INFO("test_shit\r\n");
		NRF_LOG_FLUSH();

    nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG;
    spis_config.csn_pin               = APP_SPIS_CS_PIN;
    spis_config.miso_pin              = APP_SPIS_MISO_PIN;
    spis_config.mosi_pin              = APP_SPIS_MOSI_PIN;
    spis_config.sck_pin               = APP_SPIS_SCK_PIN;

    APP_ERROR_CHECK(nrf_drv_spis_init(&spis, &spis_config, spis_event_handler));//
	
		uint32_t time_ms = 50; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;
	
	  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIMER, &timer_cfg, timer_led_event_handler);//
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER, time_ms);

    nrf_drv_timer_extended_compare(&TIMER, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_STOP_MASK, true);
		
		//GPIO init
		nrf_drv_gpiote_init();
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);

		nrf_drv_gpiote_in_init(PIN_SYN, &in_config, SynHandler);
		nrf_drv_gpiote_in_event_enable(PIN_SYN, 1);
		nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_TASK_HIGH;

		err_code = nrf_drv_gpiote_out_init(PIN_ACK, &out_config);
		if(err_code != NRF_SUCCESS)
		{
			NRF_LOG_INFO("failed to init ACK\r\n");
			NRF_LOG_FLUSH();
			return;
		}
}

void tx_set (uint8_t* tx, uint8_t* buff, uint8_t size) //Setting the buffer
{
		for(int i=0; i < size; i++)
		{
				tx[i] = buff[i];
		}	
}
void tx_clear (uint8_t* tx, uint8_t size) //Clearing buffer
{
		for(int i=0; i < size; i++)
		{
				tx[i] = 0;
		}
}  

void spi_handler(void)
{
	uint8_t temp_buf[3];
	while(1)
	{
		//int ack = 0;
		while(( syn == 0) | (command == 0))
		{
			nrf_delay_ms(50);
		}
		
		if(command)
		{
			//set timer to some time t
			nrf_drv_gpiote_out_clear(PIN_ACK);
			nrf_drv_timer_enable(&TIMER);
			
			while((syn != 0 )|(timer_!=0))
				{
					__WFE();
				}
			
			if(syn)
			{
				//prep transfer
				uint8_t *temp_tx = read_fifo();
				tx_set(m_tx_buf, temp_tx, sizeof(temp_buf));
				tx_clear(m_rx_buf, sizeof(m_rx_buf));
				spis_xfer_done = false;
				
				// SETTING ack to 0
				nrf_drv_gpiote_out_set(PIN_ACK);
				while(!spis_xfer_done)
				{
					__WFE();
				}
				return;
			}
			else
			{
				while(!spis_xfer_done)
				{
					__WFE();
				}
			}
		}
		if(syn)
		{
//			NRF_LOG_INFO("SPIS start\r\n");
//			NRF_LOG_FLUSH();
			//prepare transfer
			tx_set(m_tx_buf, temp_buf, sizeof(temp_buf));
			tx_clear(m_rx_buf, sizeof(m_rx_buf));
			spis_xfer_done = false;
			//ack = 1;
			APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length, m_rx_buf, m_length));
			nrf_drv_gpiote_out_set(PIN_ACK);
			while(!spis_xfer_done)
			{
				__WFE();
			}
			nrf_drv_gpiote_out_clear(PIN_ACK);
			NRF_LOG_INFO("ack low\r\n");
			NRF_LOG_FLUSH();
			//ack = 0;
		}
		//transfer ok?
	}
}

void fifo_init(void)
{
	d_list.max_size = 1;
	d_list.array = (uint8_t**)malloc(sizeof(uint8_t*)*d_list.max_size);
	d_list.head = d_list.tail = 0;
	d_list.used = 0;
}

void add_fifo(uint8_t *Data)
{

	if(d_list.used >= d_list.max_size)
		{
			//BLE("plz stop! I am full")
			return;
		}
		
	d_list.array[d_list.head] = Data;
	d_list.head = ((d_list.tail + 1) % d_list.max_size);
	d_list.used++;
}

uint8_t* read_fifo(void)
{
	uint8_t *Ts;
	
	//check if empty
	if(d_list.used == 0)
	{
		//print to debugger - "Nothing to read"
		return 0;
	}
	
	Ts = d_list.array[d_list.tail];
	d_list.array[d_list.tail] = 0;
	d_list.tail = ((d_list.tail + 1) % d_list.max_size);
	d_list.used--;
	return Ts;
}


