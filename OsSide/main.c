/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module   
#include "osObjects.h"                      // RTOS object definitions       
#include "data_Transfer.h"
#include "util.h"


// CMSIS RTOS header file

 

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

osThreadId print_server_tid;
osThreadDef (print_server,osPriorityLow,1,0);

extern osMailQId  (mail_q_id[5]);

/*
 * main: initialize and start the system
 */


int main (void) {
  

  osKernelInitialize ();                    // initialize CMSIS-RTOS

  
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	
	NRF_LOG_INFO("Log initialized\r\n");
	SPI_init();
	comhub_init();
	butt_init();
	NRF_LOG_FLUSH();
	tid_comhub = osThreadCreate (osThread(comhub),NULL);
	tid_SPI = osThreadCreate (osThread(SPI_controller),NULL);

  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	
	osKernelStart ();                         // start thread execution 

	while(1)
	{
		//osSignalWait(0,0);
		osDelay(100);
		NRF_LOG_FLUSH();
	}
}

