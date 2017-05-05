#include "data_Transfer.h"
#include "../util.h"



#define SPI_INSTANCE  1 //< SPI instance index. 
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  //< SPI instance.
static volatile bool spi_xfer_done;  //< Flag used to indicate that SPI instance completed the transfer.
void __svc(1) init_nothing(void);

extern osMutexId Print_Mutex;
extern osThreadId tid_SPI;

static spi_ptc_t rx_ptc_buf;
static spi_ptc_t tx_ptc_buf;

static const uint8_t m_length = sizeof(spi_ptc_t);        //< Transfer length.
static uint8_t			 STA_SYN;		//Syn state
#define PIN_SYN 30
#define PIN_ACK 31
#define CS_dac  25
#define CS_ble  SPI_SS_PIN



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
		osMutexWait(Print_Mutex,osWaitForever);
		NRF_LOG_INFO("init SPI\r\n");
		osMutexRelease(Print_Mutex);
	  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   	= 18; //SET T0 18 just to disable
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
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("failed to init SYN\r\n");
			osMutexRelease(Print_Mutex);
			return;
		}
		nrf_drv_gpiote_in_event_enable(PIN_SYN, 1);
		
		nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_TASK_HIGH;
		nrf_drv_gpiote_out_clear(PIN_ACK);
	
		err_code = nrf_drv_gpiote_out_init(PIN_ACK, &out_config);
		if(err_code != NRF_SUCCESS)
		{
			
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("failed to init ACK\r\n");
			osMutexRelease(Print_Mutex);
			return;
		}
		nrf_drv_gpiote_out_config_t CS_config = GPIOTE_CONFIG_OUT_TASK_LOW;
		nrf_drv_gpiote_out_clear(CS_ble);
		nrf_drv_gpiote_out_clear(CS_dac);
		err_code = nrf_drv_gpiote_out_init(CS_dac, &CS_config);
		if(err_code != NRF_SUCCESS)
		{
			
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("failed to init CS_dac\r\n");
			osMutexRelease(Print_Mutex);
			return;
		}
		err_code = nrf_drv_gpiote_out_init(CS_ble, &CS_config);
		if(err_code != NRF_SUCCESS)
		{
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("failed to init CS_ble\r\n");
			osMutexRelease(Print_Mutex);
			return;
		}
}

void SPI_controller(void const *argument)
{
	osMutexWait(Print_Mutex,osWaitForever);
	NRF_LOG_INFO("SPI Thread start\r\n");
	osMutexRelease(Print_Mutex);
	uint16_t wait = 50;	//wait variable, constant for now, might change later
	osEvent event_m;
	osEvent event_s;
	while(1)		// loop for mail/signal queueing, if signal recieved, prepare to recieve package, if mail prepare to send recieved mail content
	{
		
		//NRF_LOG_INFO("waiting for instruction\r\n");
		event_s = osSignalWait(1,wait);
		if (event_s.value.signals == 1)		// recieve code
		{
			
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("Received mode\r\n");
			osMutexRelease(Print_Mutex);
			//set ack hight to acknowledge send request
			
			
			//establishing connection
			//wait a moment to see if signal stays, indicating its a real signal and not a new setup
			osDelay(100);
			if(nrf_drv_gpiote_in_is_set(PIN_SYN))
			{
				
				osMutexWait(Print_Mutex,osWaitForever);
				NRF_LOG_INFO("Legit signal, waiting for syn = 0\r\n");
				osMutexRelease(Print_Mutex);
				nrf_drv_gpiote_out_set(PIN_ACK);
				
				memset(&tx_ptc_buf,0,m_length);
				memset(&rx_ptc_buf,0,m_length);
				
				//wait for spis to set syn low to autorise transfer start
				while(STA_SYN)
				{
					osDelay(50);
				}
				
				
				//connection established, transfering data
				
				osMutexWait(Print_Mutex,osWaitForever);
				NRF_LOG_INFO("Starting transfer\r\n");
				osMutexRelease(Print_Mutex);
				spi_xfer_done = false;
				start_spi_transfer(&spi, CS_ble, (uint8_t*)&tx_ptc_buf, m_length, (uint8_t*)&rx_ptc_buf, m_length);
				//set ack low to signal start of transfer
				
				
				osMutexWait(Print_Mutex,osWaitForever);
				NRF_LOG_HEXDUMP_INFO((uint8_t*)&rx_ptc_buf,m_length);
				osMutexRelease(Print_Mutex);
				
				send_mail(SPI_ID,BRN_ID,rx_ptc_buf.flgs,6,rx_ptc_buf.pld);
			}
			nrf_drv_gpiote_out_clear(PIN_ACK);
			continue;
		}
		event_m = osMailGet(SPI_Q,50);
		if (event_m.status == osEventMail)  //send code
		{
			
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("Send mode\r\n");
			osMutexRelease(Print_Mutex);
			
			mail_ptc_t *mail =(mail_ptc_t*)event_m.value.p;
			while(!nrf_drv_gpiote_in_is_set(PIN_SYN))
			{				
				nrf_drv_gpiote_out_clear(PIN_ACK);
				osDelay(50);
				//set ack high to signalise intent to transfer
				nrf_drv_gpiote_out_set(PIN_ACK);
				//wait for Syn to go high for acknowledgement of transfer
				event_s = osSignalWait(1,500);

				//if syn does not go high, set ack low, wait a moment and try again

			}
			//prepare transfer
			
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("prepare transfer\r\n");
		osMutexRelease(Print_Mutex);
			memset(&rx_ptc_buf,0,m_length);
			memcpy(tx_ptc_buf.pld,mail->pld,m_length-1);
			tx_ptc_buf.flgs = mail->flg;
			spi_xfer_done = false;
			
			nrf_drv_gpiote_out_clear(PIN_ACK);
			start_spi_transfer(&spi, CS_ble,(uint8_t*)&tx_ptc_buf, m_length, (uint8_t*)&rx_ptc_buf, m_length);
			
			
			osMutexWait(Print_Mutex,osWaitForever);
			NRF_LOG_INFO("wait for transfer done\r\n");
			osMutexRelease(Print_Mutex);


			nrf_drv_gpiote_out_clear(PIN_ACK);
			osMailFree(SPI_Q,mail);
		}
	}
}
void start_spi_transfer(nrf_drv_spi_t const * const p_instance,
																		uint8_t 				p_CS,
																		uint8_t const * p_tx_buffer,
																		uint8_t         tx_buffer_length,
																		uint8_t       * p_rx_buffer,
																		uint8_t         rx_buffer_length)
	{
		
		nrf_drv_gpiote_out_clear(p_CS);
		nrf_drv_spi_transfer(&spi, (uint8_t*)&tx_ptc_buf, m_length, (uint8_t*)&rx_ptc_buf, m_length);
		while (!spi_xfer_done)
			{
					osDelay(50);
			}
		//osDelay(1);
		nrf_drv_gpiote_out_set(p_CS);
			
		osMutexWait(Print_Mutex,osWaitForever);
		NRF_LOG_INFO("transfer done");
		osMutexRelease(Print_Mutex);
	}