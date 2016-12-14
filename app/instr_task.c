/*******************************************************************************
* Copyright (C) 2016 Mike Ferrara (mikef@mrf.sonoma.ca.us), All rights reserved.
*
*
* Filename:     instr_task.c
*
* Description: INSTR_TASK
*   
* This task de-queues packets from the CTRLinQ, dispatches based on command
* field, executes the command and acknowledges by queueing up a response packet.
* 
*******************************************************************************/
//#define TRACE_PRINT 1

#include "OSandPlatform.h"

#define GLOBAL_INSTR_TASK
#include "instr_task.h"

#include "usbcmdio.h"

// #define RX_QUEUE_DEBUG 1
// #define TX_QUEUE_DEBUG 1

uint8_t inbufBytes[256];
uint8_t outbufBytes[256];
cmd_packet_t *instrInpktBuf = (cmd_packet_t *)&inbufBytes;
cmd_packet_t *instrOutpktBuf =(cmd_packet_t *)&outbufBytes;

// return 0 or number of bytes (limited 1-255)
static int32_t readPacket(cmd_packet_t *buffer)
{
  uint_fast8_t rval = 0;
  unsigned portBASE_TYPE q_rx_stat;

  q_rx_stat = xQueueReceive(CTRLinQ, buffer, portMAX_DELAY);
  if (q_rx_stat == pdPASS) {
    rval = buffer->length;
          
#ifdef RX_QUEUE_DEBUG
    myprintf("Rx dequeue: %d bytes \r\n", rval);
#endif
  } 
  return (rval);
}

static int32_t writePacket(cmd_packet_t *buff, portTickType TMO)
{
  //compute checksum, send packet
  (void)buff;
  (void)TMO;
  return(0);
}

// this method supports the ECHO command, in the dispatcher
static void copyPacket(cmd_packet_t *inBuf, cmd_packet_t *outBuf) {
  if ((inBuf != NULL) && (outBuf != NULL)) {
    int sz = inBuf->length;
    if ((sz > 0) && (sz < 248)) {   // 248 = limit of regio values array
      memcpy(outBuf, inBuf, sz);
    }
  } // do nothing if either pointer is NULL
} // end copyPacket

__attribute__((noreturn)) portTASK_FUNCTION(vInstrumentTask, pvParameters) {

  (void)pvParameters;
  int32_t rval = 0;
  portTickType waitIfFull = 10;
  
  bzero(instrInpktBuf, sizeof(inbufBytes));  // null the storage of the packets
  bzero(instrOutpktBuf,sizeof(outbufBytes));

  while (!USBConfigured) {
    taskYIELD(); //Wait here until USB hw is configured
  }

  while (TRUE) { // If empty, yield, which gives lower-priority tasks some time
    if ((rval=readPacket(instrInpktBuf))==0) {
      taskYIELD();// no Rx DATA so yield to other threads
    } else {
      instrOutpktBuf->cmd = CMD_ACK; // default all packets to ACK-type
      instrOutpktBuf->length = USB_PKT_MIN_HEADER_SZ; // give all response packets a length

      
      switch (instrInpktBuf->cmd) {
      case CMD_ACK:
        writePacket(instrOutpktBuf,waitIfFull);
        break;

      case CMD_NAK:
        writePacket(instrOutpktBuf,waitIfFull);
        break;

      case CMD_ECHO:
        copyPacket(instrInpktBuf, instrOutpktBuf);
        writePacket(instrOutpktBuf,waitIfFull); // reuse input checksum !
        break;

      default:
        dprintf("ERROR: unrecognized or NYI command: %u\r\n",instrInpktBuf->cmd);
        instrOutpktBuf->cmd = CMD_NAK;
        writePacket(instrOutpktBuf,waitIfFull);
        break;

      } //cmd dispatcher switch
    } // if received a packet
  } // while true
} //instrument task thread

