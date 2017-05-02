/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 * @defgroup tw_scanner main.c
 * @{
 * @ingroup nrf_twi_example
 * @brief TWI Sensor Example main file.
 *
 * This file contains the source code for a sample application using TWI.
 *
 */

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "util.h"

#define NRF_LOG_MODULE_NAME "APP"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Number of possible TWI addresses. */
#define TWI_ADDRESSES      127
#define TWI_ADD 			 0x37
/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);


uint8_t curr_reg = 0xFF;
extern osMailQId mail_q_id[5];


/**
 * @brief TWI initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = 27,
       .sda                = 26,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
		
    nrf_drv_twi_enable(&m_twi);
		//pct_drv_init();
}


/**
 * @brief Function for main application entry.
 */
void twi(void)
{
    ret_code_t err_code;
    uint8_t address;
    uint8_t sample_data;
    bool detected_device = false;

    //APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_INFO("TWI scanner.\r\n");
    NRF_LOG_FLUSH();
    twi_init();

    for (address = 1; address <= TWI_ADDRESSES; address++)
    {
        err_code = nrf_drv_twi_rx(&m_twi, address, &sample_data, sizeof(sample_data));
        if (err_code == NRF_SUCCESS)
        {
            detected_device = true;
            NRF_LOG_INFO("TWI device detected at address 0x%x.\r\n", address);
        }
        NRF_LOG_FLUSH();
    }

    if (!detected_device)
    {
        NRF_LOG_INFO("No device was found.\r\n");
        NRF_LOG_FLUSH();
    }

    while (true)
    {
        /* Empty loop. */
    }
}

void pct_drv_init(pct_data_t *init)
{
	  //ret_code_t err_code;
		for(int i = 0; i < sizeof(init); i++)
		{
			pct_drv_write(init[i]);
			//twi_test(err_code);
			//NRF_LOG_INFO("%x,%x\r\n", init[i].val,init[i].reg);
			//NRF_LOG_FLUSH();
		}
		
		//APP_ERROR_CHECK(err_code);
}

void pct_drv_write(pct_data_t package)
{
		if(package.reg != curr_reg)
		{
			curr_reg = package.reg;
			nrf_drv_twi_tx(&m_twi, ((TWI_ADD<<1)|0x00), &curr_reg, sizeof(curr_reg), true);
		}
		uint8_t tempdata[2] = {package.reg,package.val[0]};
		nrf_drv_twi_tx(&m_twi, ((TWI_ADD<<1)|0x00), tempdata, sizeof(tempdata),true);
}


void pct_drv_read(pct_data_t *package)
{
		curr_reg = package->reg;
		nrf_drv_twi_tx(&m_twi, TWI_ADD, &curr_reg, 1, true);
		nrf_drv_twi_rx(&m_twi, TWI_ADD, package->val, 2/*sizeof(package->val)*/);
		
}

/** @} */

void TWI_controller(const void *arguments)
{
	osEvent evt;
	pct_data_t temp_data;
	pct_data_t *temp_ptr;
	while(1)
	{
		evt = osSignalWait(1, osWaitForever);
		if(evt.value.signals == 1)
		{
		temp_data.reg = TEMP_REG;
		temp_ptr = &temp_data;
		pct_drv_read(temp_ptr);
		uint32_t temp_p = (((temp_data.val[0]<<8)|temp_data.val[1])>>4)*125;
		uint16_t temp_d[2] = {temp_p/1000,temp_p%1000};
		NRF_LOG_INFO("Temp: %x\r\n",(temp_data.val[0]<<8)|temp_data.val[1]);
		send_mail(mail_q_id[0],TWI_ID,SPI_ID,0x00,2,temp_data.val);
		}
	}
}