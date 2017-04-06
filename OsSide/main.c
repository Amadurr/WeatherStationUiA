/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module   
#include "osObjects.h"                      // RTOS object definitions       
#include "data_Transfer.h"


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

extern osMailQId  (mail_q_in_id);

/*
 * main: initialize and start the system
 */


int main (void) {
  

  osKernelInitialize ();                    // initialize CMSIS-RTOS

  
	APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
	
	NRF_LOG_INFO("Log initialized\r\n");
	NRF_LOG_FLUSH();
	SPI_init();
	tid_comhub = osThreadCreate (osThread(comhub),NULL);
	tid_SPI = osThreadCreate (osThread(SPI_controller),NULL);

  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	
	osKernelStart ();                         // start thread execution 
	
	NRF_LOG_INFO("sending testmsg\r\n");
	NRF_LOG_FLUSH();
	uint8_t msg[] = {'b','o','o'}; 
	
	mail_protocol_t *testmsg;
	testmsg = (mail_protocol_t *) osMailAlloc(mail_q_in_id, osWaitForever);
	testmsg->sid = NULL;
	testmsg->rid = 0;
	testmsg->flg = 0;
	testmsg->pld = msg;
	osMailPut(mail_q_in_id, testmsg);
	
	while(1)
	{
		
	}
}

