
#ifndef UTIL_H__
#define UTIL_H__
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nordic_common.h"
#include "bsp.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_drv_gpiote.h"
#include "string.h"

void butt_init(void);
void print_server(void const *arg);

#endif //UTIL_H__
