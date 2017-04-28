


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

extern osMailQId  (Print_Q_id);


typedef struct{
	uint8_t *str;
	uint8_t len;
}mail_print_t;

void butt_init(void);
void print_server(void const *arg);

#endif //UTIL_H__

#ifndef U_PRINT
#define U_PRINT
#define uprint(...)\
{\
	snprintf((char*)printbuf,sizeof(printbuf),__VA_ARGS__);\
}
#endif //U_PRINT