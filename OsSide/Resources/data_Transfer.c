#include "data_Transfer.h"
#include "cmsis_os.h"                     // RTOS object definitions                                      // CMSIS RTOS header file




#define SPI_INSTANCE  1 //< SPI instance index. 
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  //< SPI instance.
static volatile bool spi_xfer_done;  //< Flag used to indicate that SPI instance completed the transfer.
void __svc(1) init_nothing(void);
pcal_interface_t *p;

static uint8_t       m_tx_buf[1];           //< TX buffer.
static uint8_t       m_rx_buf[1+1];    //< RX buffer.
static const uint8_t m_length = sizeof(m_tx_buf);        //< Transfer length.

#define PIN_SYN 30
#define PIN_ACK 31

osMutexId mut_twi;
osMutexDef(mut_twi);

//extern osThreadId tid_SPI;

extern osMailQId  *mail_pool_q_id;

/*
osMailQDef(PcalQ, 16, inMail_t);
osMailQId PcalQ_Id;

*/

void SynHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
	{
	if(action == GPIOTE_CONFIG_POLARITY_HiToLo)
	{
		NRF_LOG_INFO("syn = 0 \r\n");
		//osSignalSet(tid_SPI, 0);
	}
	if(action == GPIOTE_CONFIG_POLARITY_LoToHi)
	{
		NRF_LOG_INFO("syn = 1 \r\n");
		//osSignalSet(tid_SPI, 1);
	}
}

void tx_set( uint8_t* tx, uint8_t* buff, uint8_t size)
{
	for( int i = 0; i < size; i++)
	{
		tx[i] = buff[i];
	}
}
void tx_clr (uint8_t* tx, uint8_t size)
{
	for( int i = 0; i < size; i++)
	{
		tx[i] = 0;
	}
}


void spi_event_handler(nrf_drv_spi_evt_t const * p_event)
{
	spi_xfer_done = true;
	NRF_LOG_INFO("Transfer complete- \r\n");
	NRF_LOG_FLUSH();

	for(int i = 0; i < sizeof(m_rx_buf); i++)
	{
      NRF_LOG_INFO(" Received: \r\n");
			NRF_LOG_FLUSH();
      NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
	}
}

void SPI_init()
{
		NRF_LOG_INFO("init SPI\r\n");
		NRF_LOG_FLUSH();	
	  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;
    nrf_drv_spi_init(&spi, &spi_config, spi_event_handler);
	
	  nrf_drv_gpiote_init();
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;
	
		nrf_drv_gpiote_in_init(PIN_SYN, &in_config, SynHandler);
		nrf_drv_gpiote_in_event_enable(PIN_SYN, 1);
		nrf_drv_gpiote_out_config_t out_config = 
		{
			NRF_GPIOTE_POLARITY_TOGGLE,
			NRF_GPIOTE_INITIAL_VALUE_LOW,
			false
		};
	
		nrf_drv_gpiote_out_init(PIN_ACK, &out_config);

}

void SPI_controller(void const *argument)
{
	NRF_LOG_INFO("SPI Thread start\r\n");
	NRF_LOG_FLUSH();
	uint8_t wait = 50;	//wait variable, constant for now, might change later
	while(1)
	{
		osEvent event_m;
		osEvent event_s;
		while(1)		// loop for mail/signal queueing, if signal recieved, prepare to recieve package, if mail prepare to send recieved mail content
		{
			
      NRF_LOG_INFO("Received mode\r\n");
			NRF_LOG_FLUSH();
			event_s = osSignalWait(1,wait);
			if (event_s.value.signals == 1)		// recieve code
			{
				//set ack hight to acknowledge send request
				nrf_drv_gpiote_out_set(PIN_ACK);
				tx_clr(m_tx_buf, m_length);
				
				//wait for spis to set syn low to autorise transfer start
				event_s = osSignalWait(0,osWaitForever);\
				
				spi_xfer_done = false;
				nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length);
				//set ack low to signal start of transfer
				nrf_drv_gpiote_out_clear(PIN_ACK);
				while (!spi_xfer_done)
				{
						osDelay(50);
				}
				
				break;
			}
			event_m = osMailGet(mail_pool_q_id[0],wait);
			if (event_m.status)  //send code
			{
				NRF_LOG_INFO("Send mode\r\n");
				NRF_LOG_FLUSH();
				while(1)
				{
					//set ack high to signalise intent to transfer
					nrf_drv_gpiote_out_set(PIN_ACK);
					//wait for Syn to go high for acknowledgement of transfer
					event_s = osSignalWait(1,50);
					if(event_s.value.v == 1)
					{
						//prepare transfer
						NRF_LOG_INFO("prepare transfer\r\n");
						NRF_LOG_FLUSH();
						tx_set(m_tx_buf,(uint8_t*)event_m.value.v, m_length);
						spi_xfer_done = false;
						nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length);
						
						NRF_LOG_INFO("wait for transfer done\r\n");
						NRF_LOG_FLUSH();
						while (!spi_xfer_done)
						{
								osDelay(50);
						}
						nrf_drv_gpiote_out_clear(PIN_ACK);
						break;
					}
					//if syn does not go high, set ack low, wait a moment and try again
					nrf_drv_gpiote_out_clear(PIN_ACK);
					osDelay(50);
				}
			}
		}
	}
}


/*
void Pcal_Handler()
{
	//init TWI
	//init pcal
	osEvent evt;
	inMail_t *rptr;
	inMail_t *sptr;
	PcalQ_Id = osMailCreate(osMailQ(PcalQ),NULL);
	while(1)
	{
		evt = osMailGet(PcalQ_Id, osWaitForever);
		if(evt.status == osEventMail)
		{
			rptr = evt.value.p;
			if(rptr->rId == 0x01)
			{
				if((rptr->flags&0x01) == 1)
				{
					//getdata
					pcal_drv_read(rptr->p);
					{
						
					}
				}
				else
				{
					//writedata
				}
			}
		}
	}
}
*/


