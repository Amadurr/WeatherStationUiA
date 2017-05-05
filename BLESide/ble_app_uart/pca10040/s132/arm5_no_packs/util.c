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
#include "BLEFunctions.h"
#include "nrf_drv_uart.h"
#include <stdarg.h>

static nrf_drv_uart_t p_uart = NRF_DRV_UART_INSTANCE(0);
static nrf_drv_uart_config_t p_uart_config = NRF_DRV_UART_DEFAULT_CONFIG;


uint8_t printbuf[50];

#define SPIS_INSTANCE 1 /**< SPIS instance index. */
static const nrf_drv_spis_t spis = NRF_DRV_SPIS_INSTANCE(SPIS_INSTANCE);/**< SPIS instance. */
const nrf_drv_timer_t TIMER = NRF_DRV_TIMER_INSTANCE(1);
volatile uint8_t timer_;

uint8_t command;
uint8_t ble_msg[6];


static const uint8_t m_length = sizeof(spi_ptc_t);        	/**< Transfer length. */
static uint8_t       m_tx_buf[m_length];           					/**< TX buffer. */
static uint8_t       m_rx_buf[m_length + 1];   							/**< RX buffer. */
static volatile bool spis_xfer_done; /**< Flag used to indicate that SPIS instance completed the transfer. */
spi_ptc_t rx_ptc_buf;
spi_ptc_t tx_ptc_buf;

fifo_list_t d_list; 									 /**< Legger til struct fifo_list. */
uint8_t syn = 0x00;
#define PIN_SYN 31
#define PIN_ACK 30

//Handler for SPI evenets
void spis_event_handler(nrf_drv_spis_event_t event)
{
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE)
    {
			spis_xfer_done = true;
        uprint(" Transfer completed. Received: %s\r\n",(char*)m_rx_buf);
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
	if((pin == PIN_SYN) && (action == NRF_GPIOTE_POLARITY_TOGGLE))
	{
		syn = nrf_drv_gpiote_in_is_set(pin);
		uprint("syn = %i\r\n", syn);
	}
}
		
void spi_app_init(void)
{
		uprint("spi init\r\n");

    nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG;
    spis_config.csn_pin               = APP_SPIS_CS_PIN;
    spis_config.miso_pin              = APP_SPIS_MISO_PIN;
    spis_config.mosi_pin              = APP_SPIS_MOSI_PIN;
    spis_config.sck_pin               = APP_SPIS_SCK_PIN;

    APP_ERROR_CHECK(nrf_drv_spis_init(&spis, &spis_config, spis_event_handler));//
	
		uint32_t time_ms = 500; //Time(in miliseconds) between consecutive compare events.
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
	  in_config.pull = GPIO_PIN_CNF_PULL_Pullup;

		nrf_drv_gpiote_in_init(PIN_SYN, &in_config, SynHandler);
		nrf_drv_gpiote_in_event_enable(PIN_SYN, 1);
		nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_TASK_HIGH;
		nrf_drv_gpiote_out_clear(PIN_ACK);

		err_code = nrf_drv_gpiote_out_init(PIN_ACK, &out_config);
		if(err_code != NRF_SUCCESS)
		{
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
	
	fifo_data_t tmp_data;
	fifo_data_t *ttp = &tmp_data;		
	while(1)
	{
		uprint("stating new cycle\r\n");
		//int ack = 0;
		while((!syn) && (!command))
		{
			__WFE();
		}
		
		//Sending mode
		uprint("syn: %x, com: %x\r\n",syn,command);
		if(command)
		{
			command = 0;
			//set timer to some time t
			nrf_drv_gpiote_out_set(PIN_ACK);
			nrf_drv_timer_enable(&TIMER);
			uprint("sedning mode\r\n");
			
			while(( !syn ) && ( !timer_ ))
			{
				__WFE();
			}
			if(timer_)
			{
				uprint("syn timeout, waiting to recieve package\r\n");
				
				spis_xfer_done = false;

				nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length, m_rx_buf, m_length);
				while(!spis_xfer_done)
				{
					__WFE();
				}
			}
			//prep transfer
			nrf_delay_ms(100);
			//pointer to retrieve data from fifo	
			read_fifo(ttp);
			//inserting data to the spi protocol
			memcpy(&tx_ptc_buf,&tmp_data,m_length);	//pointer to tx buffer
			//copying the data from the spi protocol buffer to the transfer buffer
			memset(&rx_ptc_buf,0,m_length);
			
			uprint("blep\r\n");
			uprint("sending data: %s\r\n", (uint8_t*)m_tx_buf);
			
			spis_xfer_done = false;

			nrf_drv_spis_buffers_set(&spis, (uint8_t*)&tx_ptc_buf, m_length, (uint8_t*)&rx_ptc_buf, m_length);
			//NRF_LOG_INFO("buffers set\r\n", (uint32_t)m_tx_buf);
			//NRF_LOG_FLUSH();
			nrf_drv_gpiote_out_clear(PIN_ACK);
							
			uprint("waiting for spi transfer\r\n");
			while(!spis_xfer_done)
			{
				__WFE();
			}
			uprint("transfer done\r\n");
			
			
			continue;
		}
		//Recieve mode
		if(syn)
		{
//			NRF_LOG_INFO("SPIS start\r\n");
//			NRF_LOG_FLUSH();
			//prepare transfer
			nrf_delay_ms(100);
			if(!syn)
			{			
				continue;
			}
			//tx_clear(m_tx_buf, m_length);
			//tx_clear(m_rx_buf, sizeof(m_rx_buf));
			memset(&tx_ptc_buf,0,m_length);
			memset(&rx_ptc_buf,0,m_length);
			
			spis_xfer_done = false;
			//ack = 1;
			APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, (uint8_t*)&tx_ptc_buf, m_length, (uint8_t*)&rx_ptc_buf, m_length));
			nrf_drv_gpiote_out_set(PIN_ACK);
			nrf_delay_ms(50);

			while(!spis_xfer_done)
			{
				__WFE();
			}
			nrf_drv_gpiote_out_clear(PIN_ACK);
			uprint("ack low\r\n");
			spi_eval(rx_ptc_buf);
			//ble_print(m_rx_buf);
				
			//ack = 0;			
		}
		//transfer ok?
		power_manage();
	 }
}

void fifo_init(void)
{
	d_list.max_size = 6;
	d_list.array = (fifo_data_t*)malloc(sizeof(fifo_data_t)*d_list.max_size);
	d_list.head = 0;
	d_list.tail = 0;
	d_list.used = 0;
}

void add_fifo(fifo_data_t *buf)
{

		if(d_list.used >= d_list.max_size)
			{
				//BLE("plz stop! I am full")
				return;
			}
		memcpy(&d_list.array[d_list.head],buf,m_length);
		d_list.head = ((d_list.head + 1) % d_list.max_size);
		d_list.used++;
}

void read_fifo(fifo_data_t *buf)
{
		//check if empty
		if(d_list.used == 0)
		{
			//print to debugger - "Nothing to read"
			return;
		}
		
		memcpy(buf,&(d_list.array[d_list.tail]),7);
		d_list.tail = ((d_list.tail + 1) % d_list.max_size);
		d_list.used--;
		return;
}
void uart_print_init(void)
{
			                           
		p_uart_config.pseltxd            = TX_PIN_NUMBER;                           \
		p_uart_config.pselrxd            = RX_PIN_NUMBER;                            \
		p_uart_config.pselcts            = CTS_PIN_NUMBER;                            \
		p_uart_config.pselrts            = RTS_PIN_NUMBER;   
	
    nrf_drv_uart_init(&p_uart, &p_uart_config, NULL);
}

void uartprint(uint8_t *str)
{
	/*
		for (uint32_t i = 0; i < strlen((char*)str); i++)
		{
				while (app_uart_put(str[i]) != NRF_SUCCESS);
		}*/
	//nrf_drv_uart_tx(&p_uart, (const uint8_t *)str, strlen((char*)str));
	//printbuf[0] = '\0';
	
}
