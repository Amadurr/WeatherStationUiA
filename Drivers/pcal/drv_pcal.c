#include "drv_pcal.h"

#define NRF_LOG_MODULE_NAME "APP"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
static uint8_t led_State = 0xFF;
extern pcal_interface_t *p;

void pcal_drv_init(pcal_data_t *init)
{
	  ret_code_t err_code;
		for(int i = 0; i < sizeof(init); i++)
		{
			pcal_drv_write(init[i]);
			twi_test(err_code);
			NRF_LOG_INFO("%x,%x\r\n", init[i].val,init[i].reg);
			NRF_LOG_FLUSH();
		}
		
		APP_ERROR_CHECK(err_code);
}

void pcal_drv_write(pcal_data_t package)
{
		uint8_t tempdata[2] = {package.reg,package.val};
		nrf_drv_twi_tx(p->twi, p->addr, tempdata, sizeof(tempdata),true);
}

void pcal_drv_read(pcal_data_t *package)
{
		nrf_drv_twi_tx(p->twi, p->addr, (uint8_t*)&package->reg, sizeof(package->reg), true);
		nrf_drv_twi_rx(p->twi, p->addr, &package->val, sizeof(package->val));
}

void pcal_drv_led_on(uint8_t led_On)
{
		led_On ^= 0xF0;
		led_State = led_On & led_State;
		pcal_data_t temp_data = {OUTPUT_PORT, led_State};
		pcal_drv_write(temp_data);
}

void pcal_drv_led_off(uint8_t led_Off)
{
		led_State = led_Off | led_State;
		pcal_data_t temp_data = {OUTPUT_PORT, led_State};
		pcal_drv_write(temp_data);
}


