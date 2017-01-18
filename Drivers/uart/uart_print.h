#ifndef INCLUSION_GUARD_UART_PRINT_H
#define INCLUSION_GUARD_UART_PRINT_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "bsp.h"
#include "nordic_common.h"
#include "nrf_drv_uart.h"


ret_code_t UartPrintInit(void);
void UartPrintf(const char* fmt, ...);
void UartPrint();

typedef struct{
	char buffer[50];
}print_f;

#endif //INCLUSION_GUARD_UART_PRINT_H
