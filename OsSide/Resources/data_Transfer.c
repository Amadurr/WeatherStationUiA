#include "data_Transfer.h"
#include "cmsis_os.h"                     // RTOS object definitions                                      // CMSIS RTOS header file
#include "nrf_drv_spi.h"
#include "drv_pcal.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */
void __svc(1) init_nothing(void);

static uint8_t       m_tx_buf[1];           /**< TX buffer. */
static uint8_t       m_rx_buf[1+1];    /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */
static uint8_t			 butt_Changes[1];
osMutexId mut_twi;
osMutexDef(mut_twi);
extern osThreadId tid_SPI;


osMessageQId switch_butt_q;
osMessageQDef(switch_butt_q, 0x10, unsigned_int);

void spi_event_handler(nrf_drv_spi_evt_t const * p_event)
{
	spi_xfer_done = true;
	//NRF_LOG_INFO("Transfer complete- \r\n");

	for( int i = 0; i < sizeof(m_rx_buf); i++)
	{
	}
}

void SPI_init()
{
	  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;
    nrf_drv_spi_init(&spi, &spi_config, spi_event_handler);

}

void SPI_controller()
{
	while(1)
	{
		osEvent event = osMessageGet(switch_butt_q, 0);
		if(event.status)
		{
			memset(m_rx_buf, event.value.v, m_length);
		}
		else
		{
			memset(m_rx_buf, 0, m_length);
		}
		spi_xfer_done = false;
		
		//APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length));
		nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, m_rx_buf, m_length);
		
		while (!spi_xfer_done)
    {
        __WFE();
    }
		
		//NRF_LOG_FLUSH();
		
	}
}

void buttEncoder()
{
  uint8_t old_data;
	uint8_t new_data=0x00;
	uint8_t mask = 0x01;
	while(mask < 0x10)
	{
		if((mask&old_data) < (mask&new_data))
		{
			butt_Changes[0] |= mask;
		}
		if((mask&old_data) > (mask&new_data))
		{
			butt_Changes[0] |= (mask<< 4);
		}
		mask <<= 0x1;
	}
	old_data = new_data;
}

void __SVC_1			(void)
{
	return;
}