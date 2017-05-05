#ifndef SAADC_H__
#define SAADC_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_drv_saadc.h"
#include "boards.h"
#include "app_error.h"
#include "app_util_platform.h"
#include <string.h>

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
void saadc_main(void);


#endif //SAADC_H__
