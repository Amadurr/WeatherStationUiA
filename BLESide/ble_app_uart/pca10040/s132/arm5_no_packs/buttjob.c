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

uint8_t data[] =	"ABC";
extern uint8_t command;
#define BUTT 13

/*
void butt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	command = 1;
	add_fifo(data);
	uprint("send request data: %s\r\n", (uint32_t)data);
}

void butt_init(void)
{    
	ret_code_t err_code;

	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
	in_config.pull = NRF_GPIO_PIN_PULLUP;

	err_code = nrf_drv_gpiote_in_init(BUTT, &in_config, butt_handler);
	APP_ERROR_CHECK(err_code);

	nrf_drv_gpiote_in_event_enable(BUTT, true);

}*/
