/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
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
 * @defgroup nrf_adc_example main.c
 * @{
 * @ingroup nrf_adc_example
 * @brief ADC Example Application main file.
 *
 * This file contains the source code for a sample application using ADC.
 *
 * @image html example_board_setup_a.jpg "Use board setup A for this example."
 */


#include "saadc.h"
#include "util.h"
#include "data_Transfer.h"

extern osMailQId mail_q_id[5];
extern osMutexId Print_Mutex;


#define SAMPLES_IN_BUFFER 1
volatile uint8_t state = 1;
uint8_t Vdc;
static nrf_saadc_value_t     m_buffer_pool[2][SAMPLES_IN_BUFFER];





void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
	

			nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
			Vdc = p_event->data.done.p_buffer[0]*36/10;
			

}


void saadc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN3);

    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
	
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);


}


/**
 * @brief Function for main application entry.
 */

void saadc_main(void)
{		

    NRF_LOG_INFO("SAADC HAL simple example.\r\n");
    saadc_init();
}


/** @} */
