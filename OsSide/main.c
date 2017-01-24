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
osThreadId tid_SPI;
osThreadDef (SPI,osPriorityNormal,1,0);
*/

/*
 * main: initialize and start the system
 */


int main (void) {
  

  osKernelInitialize ();                    // initialize CMSIS-RTOS


  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	
	osKernelStart ();                         // start thread execution 
}
