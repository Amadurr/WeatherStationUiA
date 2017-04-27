#include "data_Transfer.h"
#include "cmsis_os.h"                     // RTOS object definitions                                      // CMSIS RTOS header file




#define SPI_INSTANCE  1 //< SPI instance index. 
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  //< SPI instance.
static volatile bool spi_xfer_done;  //< Flag used to indicate that SPI instance completed the transfer.
void __svc(1) init_nothing(void);
pcal_interface_t *p;

extern osThreadId tid_SPI;

static uint8_t       m_tx_buf[3];           //< TX buffer.
static uint8_t       m_rx_buf[3+1];    //< RX buffer.
static const uint8_t m_length = sizeof(m_tx_buf);        //< Transfer length.
static uint8_t			 STA_SYN;		//Syn state
#define PIN_SYN 30
#define PIN_ACK 31

osMutexId mut_log;
osMutexDef(mut_log);

//extern osThreadId tid_SPI;

extern osMailQId mail_q_id[5];

/*
osMailQDef(PcalQ, 16, inMail_t);
osMailQId PcalQ_Id;

*/

void SynHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
{
	if((pin == PIN_SYN) && (action == NRF_GPIOTE_POLARITY_TOGGLE))
	{
		if(nrf_drv_gpiote_in_is_set(pin))
		{
			STA_SYN = 1;
		}
		else
		{
			STA_SYN = 0;
		}
		NRF_LOG_INFO("syn = %i \r\n",STA_SYN);
		osSignalSet(tid_SPI, STA_SYN);
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
}

void SPI_init(void)
{
		ret_code_t err_code;
		NRF_LOG_INFO("init SPI\r\n");
	  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   	= SPI_SS_PIN;
    spi_config.miso_pin 	= SPI_MISO_PIN;
    spi_config.mosi_pin 	= SPI_MOSI_PIN;
    spi_config.sck_pin  	= SPI_SCK_PIN;
		spi_config.frequency	= NRF_DRV_SPI_FREQ_2M;
    nrf_drv_spi_init(&spi, &spi_config, spi_event_handler);
	
	  nrf_drv_gpiote_init();
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
	  in_config.pull = GPIO_PIN_CNF_PULL_Pullup;
	
		err_code = nrf_drv_gpiote_in_init(PIN_SYN, &in_config, SynHandler);
		if(err_code != NRF_SUCCESS)
		{
			NRF_LOG_INFO("failed to init ACK\r\n");
			return;
		}
		nrf_drv_gpiote_in_event_enable(PIN_SYN, 1);
		nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_TASK_HIGH;
		nrf_drv_gpiote_out_clear(PIN_ACK);
	
		err_code = nrf_drv_gpiote_out_init(PIN_ACK, &out_config);
		if(err_code != NRF_SUCCESS)
		{
			NRF_LOG_INFO("failed to init ACK\r\n");
			return;
		}
}

void SPI_controller(void const *argument)
{
	NRF_LOG_INFO("SPI Thread start\r\n");
	uint16_t wait = 50;	//wait variable, constant for now, might change later
	osEvent event_m;
	osEvent event_s;
	uint8_t i;
	while(1)		// loop for mail/signal queueing, if signal recieved, prepare to recieve package, if mail prepare to send recieved mail content
	{
		
		//NRF_LOG_INFO("waiting for instruction\r\n");
		event_s = osSignalWait(1,wait);
		if (event_s.value.signals == 1)		// recieve code
		{
			NRF_LOG_INFO("Received mode\r\n");
			//set ack hight to acknowledge send request
			
			//wait a moment to see if signal stays, indicating its a real signal and not a new setup
			osDelay(100);
			if(nrf_drv_gpiote_in_is_set(PIN_SYN))
			{
				NRF_LOG_INFO("Legit signal, waiting for syn = 0\r\n");
				nrf_drv_gpiote_out_set(PIN_ACK);
				tx_clr(m_tx_buf, m_length);
				
				//wait for spis to set syn low to autorise transfer start
				while(STA_SYN)
				{
					osDelay(50);
				}
				NRF_LOG_INFO("Starting transfer\r\n");
				spi_xfer_done = false;
				nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length);
				//set ack low to signal start of transfer
				while (!spi_xfer_done)
				{
						osDelay(50);
				}
				NRF_LOG_INFO("transfer done, recieved:\r\n");
				i = 0;
				while(m_rx_buf[i])
				{
					NRF_LOG_INFO("%c\r\n",m_rx_buf[i]);
					i++;
				}
			}
			nrf_drv_gpiote_out_clear(PIN_ACK);
			continue;
		}
		event_m = osMailGet(mail_q_id[1],50);
		if (event_m.status == osEventMail)  //send code
		{
			NRF_LOG_INFO("Send mode\r\n");
			mail_protocol_t *mail =(mail_protocol_t*)event_m.value.p;
			while(!nrf_drv_gpiote_in_is_set(PIN_SYN))
			{				
				nrf_drv_gpiote_out_clear(PIN_ACK);
				osDelay(500);
				//set ack high to signalise intent to transfer
				nrf_drv_gpiote_out_set(PIN_ACK);
				//wait for Syn to go high for acknowledgement of transfer
				event_s = osSignalWait(1,500);

				//if syn does not go high, set ack low, wait a moment and try again

			}
			//prepare transfer
			NRF_LOG_INFO("prepare transfer\r\n");
			tx_set(m_tx_buf,(uint8_t*)mail->pld, m_length);
			spi_xfer_done = false;
			nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length);
			
			NRF_LOG_INFO("wait for transfer done\r\n");
			while (!spi_xfer_done)
			{
					osDelay(50);
			}
			NRF_LOG_INFO("transfer done");
			nrf_drv_gpiote_out_clear(PIN_ACK);
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


