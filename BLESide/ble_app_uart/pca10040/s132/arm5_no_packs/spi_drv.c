#include "util.h"
#include "definitions.h"

extern ble_nus_t m_nus; 
void spi_eval(spi_ptc_t msg)
{
	uint8_t str[BLE_NUS_MAX_DATA_LEN];
	switch(msg.flgs&0x0f)
	{
		case (0x01):
		{
		
			int16_t temp_p = ((msg.pld[0]<<8)|msg.pld[1]);

			int32_t temp_d0 = (temp_p*125)/32;
			int32_t temp_d1 = temp_d0/1000;
			int32_t temp_d2 =	abs(temp_d0 - (temp_d1*1000));
			snprintf((char *)str, BLE_NUS_MAX_DATA_LEN, "Temp: %i,%i C\r\n", temp_d1,temp_d2);
			if(msg.flgs&0x80)
			{
				ble_nus_string_send(&m_nus, str, strlen((char*)str));
			}
			if(msg.flgs&0x40)
			{
				for (uint32_t i = 0; i < strlen((char*)str)+1; i++)
				{
					while (app_uart_put(str[i]) != NRF_SUCCESS);
				}	
			}
			break;
		}
		case (0x02):
		{
			
			snprintf((char *)str, BLE_NUS_MAX_DATA_LEN, "V_adc: %i mV\r\n", msg.pld[0]);
			if(msg.flgs&0x80)
			{
				ble_nus_string_send(&m_nus, str, strlen((char*)str));
			}
			if(msg.flgs&0x40)
			{
				for (uint32_t i = 0; i < strlen((char*)str)+1; i++)
				{
					while (app_uart_put(str[i]) != NRF_SUCCESS);
				}	
			}
		}
		case (0x04):
		case (0x08):
		default:
			break;
	}
}