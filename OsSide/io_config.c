#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "switch_monitor.h"
#include "nordic_common.h"
#include "cmsis_os.h"
#include "uart_print.h"
#include "string.h"
#include "lamp_control.h"

#define PIN_SYN 30
#define PIN_ACK 30
extern osThreadId tid_SPI;


void SPI_init()
{

    nrf_drv_gpiote_init();
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;
		nrf_drv_gpiote_in_init(PIN_SYN, &in_config, interrupt_handler);
		nrf_drv_gpiote_in_event_enable(PIN_SYN, 1);
	
	
		nrf_gpio_cfg_output(PIN_ACK)
		NRF_GPIO->OUTCLR(PIN_ACK)
	
    switch_msg_q = osMessageCreate(osMessageQ(switch_msg_q), NULL);
		uart_mutex_2 = osMutexCreate(osMutex(uart_mutex_2));
}


void SynHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
	{
	if(action = GPIOTE_CONFIG_POLARITY_HiToLo)
		osSignalPut(tid_SPI, 0);
	if(action = GPIOTE_CONFIG_POLARITY_LoToHi)
		osSignalPut(tid_SPI, 1);
}