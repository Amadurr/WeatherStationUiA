/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module   
#include "osObjects.h"                      // RTOS object definitions       
#include "data_Transfer.h"
#include "util.h"
#include "saadc.h"

// CMSIS RTOS header file
osMutexDef(Print_Mut);
osMutexId (Print_Mutex);
 

/*
osThreadId tid_dataSender;
osThreadDef (dataSender,osPriorityNormal,1,0);
osThreadId tid_dataReciever;
osThreadDef (dataReciever,osPriorityNormal,1,0);
*/

osThreadId tid_SPI;
osThreadDef (SPI_controller,osPriorityNormal,1,0);

osThreadId tid_comhub;
osThreadDef (comhub,osPriorityNormal,1,0);

osThreadId tid_brn;
osThreadDef (MainControlUnit,osPriorityNormal,1,0);

osThreadId tid_TWI;
osThreadDef (TWI_controller,osPriorityNormal,1,0);

osThreadId tid_DAC;
osThreadDef (DAC_controller,osPriorityNormal,1,0);


extern osMailQId  (mail_q_id[5]);

/*
 * main: initialize and start the system
 */


int main (void) {
  

  osKernelInitialize ();                    // initialize CMSIS-RTOS

  
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	Print_Mutex = osMutexCreate(osMutex(Print_Mut));
	osMutexWait(Print_Mutex,osWaitForever);
	NRF_LOG_INFO("Log initialized\r\n");
	osMutexRelease(Print_Mutex);
	SPI_init();
	comhub_init();
	//butt_init();
	NRF_LOG_FLUSH();
	tid_comhub = osThreadCreate (osThread(comhub),NULL);
	tid_SPI = osThreadCreate (osThread(SPI_controller),NULL);
	tid_brn = osThreadCreate (osThread(MainControlUnit),NULL);
	tid_TWI = osThreadCreate (osThread(TWI_controller),NULL);
	tid_DAC = osThreadCreate (osThread(DAC_controller),NULL);
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	
	osKernelStart ();                         // start thread execution 
	/*
	uint8_t msg[] = {'A','B','C'}; 
	mail_protocol_t *testmsg;
	testmsg = (mail_protocol_t *) osMailAlloc(mail_q_id[0], osWaitForever);
	if(testmsg == NULL)
	{
		NRF_LOG_INFO("failed to make mail\r\n")
		
	}
	testmsg->sid = 255;
	testmsg->rid = 0;
	testmsg->flg = 0;
	testmsg->pld_s = sizeof(msg);
	testmsg->pld = msg;
	osMailPut(mail_q_id[0], testmsg);*/
	//twi();
	
  twi_init();
	nrf_drv_gpiote_out_config_t test_pin = GPIOTE_CONFIG_OUT_TASK_HIGH;
	nrf_drv_gpiote_out_init(7, &test_pin);
	nrf_drv_gpiote_out_set(7);
	saadc_main();

	
	while(1)
	{
		//osSignalWait(0,0);
		osDelay(100);
		NRF_LOG_FLUSH();
	}
}

