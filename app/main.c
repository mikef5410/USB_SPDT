#define GLOBAL_OSandPlatform
#include "OSandPlatform.h"

#include "debug_shell.h"
#include "instr_task.h"

xTaskHandle *xLED1TaskHandle;
xTaskHandle *xStackTaskHandle;
xTaskHandle *xUSBCDCACMTaskHandle;
xTaskHandle *xDebugShellTaskHandle;
xTaskHandle *xInstrumentTaskHandle;

extern portTASK_FUNCTION(vUSBCDCACMTask, pvParameters); //in cdcacm.c
extern portTASK_FUNCTION(vStackTask, pvParamaters);
extern portTASK_FUNCTION(vInstrumentTask, pvParameters);
static portTASK_FUNCTION(vLEDTask1, pvParameters)
{
  (void)(pvParameters);//unused params
  while(1) {
    if (USBConfigured) {
      redOn(0);
      greenOn(1);
      delayms(50);
      greenOn(0);
      delayms(100);
      greenOn(1);
      delayms(50);
      greenOn(0);
      delayms(800);
    } else {
      redOn(1);
      taskYIELD();
    }
  }
}



int main(void)
{
  portBASE_TYPE qStatus = pdPASS;   // = 1, this, and pdFAIL = 0, are in projdefs.h

  // Now setup the clocks ...
  setupClocks();
  
  // Setup GPIOs
  setupGPIOs();


  redOn(1); greenOn(1);
  Delay(0x8FFFFF);
  redOn(0); greenOn(0);

  init_hiresTimer();
  
  //Fixup NVIC for FreeRTOS ...
  setupNVIC();
  
  // Create tasks
  // remember, stack size is in 32-bit words and is allocated from the heap ...
  qStatus = xTaskCreate(vLEDTask1, "LED Task 1", 64, NULL, (tskIDLE_PRIORITY + 1UL),
                        (xTaskHandle *) &xLED1TaskHandle);


  qStatus = xTaskCreate(vStackTask, "Stacklight", 128, NULL, (tskIDLE_PRIORITY + 1UL),
                        (xTaskHandle *) &xStackTaskHandle);


  delayms(150);
  qStatus = xTaskCreate(vUSBCDCACMTask, "USB Serial Task", 256, NULL, (tskIDLE_PRIORITY + 1UL),
                        (xTaskHandle *) &xUSBCDCACMTaskHandle);

  qStatus = xTaskCreate(vDebugShell, "Debug shell", 1024, NULL, (tskIDLE_PRIORITY + 1UL),
                        (xTaskHandle *) &xDebugShellTaskHandle);

  qStatus = xTaskCreate(vInstrumentTask, "Instrument task", 1024, NULL,  (tskIDLE_PRIORITY + 1UL),
                        (xTaskHandle *) &xInstrumentTaskHandle);
  
  (void) qStatus;

  // start the scheduler
  vTaskStartScheduler();

  /* Control should never come here */
  //DEBUGSTR("Scheduler Failure\n");
  while (1) {}
}
  



void vApplicationStackOverflowHook( xTaskHandle xTask __attribute__(( unused )), signed char *pcTaskName __attribute__(( unused )) )
{
  while (1) {
  }  
  return;
}

void vApplicationMallocFailedHook( void ) {
  while (1) {
  }  
  return;
}

//	vApplicationIdleHook() ...
// 
//  will only be called if configUSE_IDLE_HOOK is set to 1 in FreeRTOSConfig.h.
//  It will be called on each iteration of the idle task.
//
//	It is essential that code added to this hook function never attempts
//	to block in any way (for example, call xQueueReceive() with a block time
//	specified, or call vTaskDelay()).  If the application makes use of the
//	vTaskDelete() API function (as this demo application does) then it is also
//	important that vApplicationIdleHook() is permitted to return to its calling
//	function, because it is the responsibility of the idle task to clean up
//	memory allocated by the kernel to any task that has since been deleted.
//
void vApplicationIdleHook( void ) {
  // jjones:  I have configured to use the IdleHook, but don't
  //          really have anything for it do do... yet.
  //          The default behavior is just to surrender to the tick ...
  __WFI();
  // What is shown below is from an LWIP and USB-CDC demo, where the idle task
  // lazy-dumps characters out the USB port.  I include it as a tutorial on
  // using the TickCount (or some other higher-res timer) to  prevent the
  // idle task from churning, or sending any faster than necessary.

  //  static portTickType xLastTx = 0;
  //  char cTxByte;
  //  
  //  /* The idle hook simply sends a string of characters to the USB port.
  //  	 The characters will be buffered and sent once the port is connected. */
  //  if( ( xTaskGetTickCount() - xLastTx ) > mainUSB_TX_FREQUENCY ) {
  //  	xLastTx = xTaskGetTickCount();
  //  	for( cTxByte = mainFIRST_TX_CHAR; cTxByte <= mainLAST_TX_CHAR; cTxByte++ ) {
  //  	  vUSBSendByte( cTxByte );
  //  	}		
  //  }
} // end vApplicationIdleHook


// FreeRTOS application tick hook 
void vApplicationTickHook(void)
{}

void _exit(int status __attribute__(( unused )) )
{
  //TRIG_HARDFAULT;
  while (1) ;
}
