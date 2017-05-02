#include "drv_pcal.h"

/*
#define NRF_LOG_MODULE_NAME "APP"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
*/
static uint8_t led_State = 0xFF;
extern pct_interface_t *p;

static uint8_t curr_reg = NULL;


void pct_drv_init(pct_data_t *init)
{
	  //ret_code_t err_code;
		for(int i = 0; i < sizeof(init); i++)
		{
			pcal_drv_write(init[i]);
			//twi_test(err_code);
			//NRF_LOG_INFO("%x,%x\r\n", init[i].val,init[i].reg);
			//NRF_LOG_FLUSH();
		}
		
		//APP_ERROR_CHECK(err_code);
}

void pct_drv_write(pct_data_t *package)
{
		if(package->reg != curr_reg)
		{
			curr_reg = package->reg;
			nrf_drv_twi_tx(p->twi, (p->addr<<1|0x00), (uint8_t*)&curr_reg, sizeof(curr_reg), true);
		}
		uint8_t tempdata[2] = {package->reg,package->val};
		nrf_drv_twi_tx(p->twi, (p->addr<<1|0x00), tempdata, sizeof(tempdata),true);
}


void pct_drv_read(pct_data_t *package)
{
		
		if(package->reg != curr_reg)
		{
			curr_reg = package->reg;
			nrf_drv_twi_tx(p->twi, (p->addr<<1|0x00), (uint8_t*)&curr_reg, sizeof(curr_reg), true);
		}
		nrf_drv_twi_rx(p->twi, (p->addr<<1|0x01), &package->val, sizeof(package->val));
		

}

