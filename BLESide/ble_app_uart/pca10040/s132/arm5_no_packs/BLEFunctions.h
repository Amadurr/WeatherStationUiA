#ifndef __BLEFUNCTIONS_H
#define __BLEFUNCTIONS_H

#include "definitions.h"

void ble_app_init(void);

void uart_init(void);
void power_manage(void);

void buttons_leds_init(bool * p_erase_bonds);
void advertising_init(void);
void uart_event_handle(app_uart_evt_t * p_event);
void bsp_event_handler(bsp_event_t event);

void ble_stack_init(void);
void ble_evt_dispatch(ble_evt_t * p_ble_evt);
void on_ble_evt(ble_evt_t * p_ble_evt);
void on_adv_evt(ble_adv_evt_t ble_adv_evt);
void sleep_mode_enter(void);
void conn_params_init(void);
void conn_params_error_handler(uint32_t nrf_error);
void on_conn_params_evt(ble_conn_params_evt_t * p_evt);
void services_init(void);
void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length);
void gap_params_init(void);
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);


#endif
