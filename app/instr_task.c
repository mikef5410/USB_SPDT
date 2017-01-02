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

// #define RX_QUEUE_DEBUG 1
// #define TX_QUEUE_DEBUG 1
extern usbd_device *CDCACM_dev; //cdcacm.c

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
    rval = le16toh(buffer->length);
          
#ifdef RX_QUEUE_DEBUG
    myprintf("Rx dequeue: %d bytes \r\n", rval);
#endif
  } 
  return (rval);
}

//returns bytes written or 0 if failed.
static int32_t writePacket(cmd_packet_t *buff, portTickType TMO)
{
  FLETCHER_CHECKSUM sums;
  uint16_t rval=0;
  portTickType timeLeft = TMO;
  int32_t j;
  uint8_t *buffbytes = (uint8_t *)buff;
  
  sums.Checksum1=0;
  sums.Checksum2=0;

  //compute checksum, send packet
  redOn(1);
  buff->checksum=0;
  for (j=1; j<(le16toh(buff->length)-1); j++) {
    FLETCH(&sums,buffbytes[j]);
  }
  buff->checksum = (uint16_t) sums.Checksum2*256 + sums.Checksum1;

  if (TMO==0) timeLeft=10;
  rval=usbd_ep_write_packet(CDCACM_dev, 0x81, buffbytes+1, le16toh(buff->length)); //skip the flow byte
  while (rval==0 && timeLeft>1) {
    if (rval == 0) {
      delayms(1);
      if (TMO) timeLeft-=1;
    }
    rval=usbd_ep_write_packet(CDCACM_dev, 0x81, buffbytes+1, le16toh(buff->length)); //skip the flow byte
  }
  redOn(0);
  return(rval);
}

// this method supports the ECHO command, in the dispatcher
static void copyPacket(cmd_packet_t *inBuf, cmd_packet_t *outBuf) {
  if ((inBuf != NULL) && (outBuf != NULL)) {
    int sz = le16toh(inBuf->length) + 1;
    if ((sz > 0) && (sz < 248)) {   // 248 = limit of regio values array
      memcpy(outBuf, inBuf, sz);
    }
  } // do nothing if either pointer is NULL
} // end copyPacket

__attribute__((noreturn)) portTASK_FUNCTION(vInstrumentTask, pvParameters) {

  (void)pvParameters;
  int32_t rval = 0;
  portTickType waitIfFull = 10;
  attenSetting_t atten;
  spdtSel_t swSel;
  spdtSetting_t swState;
  sp8tSel_t muxSel;
  uint8_t color;
  uint32_t onTime;
  uint32_t offTime;
  uint32_t count;
  
  bzero(instrInpktBuf, sizeof(inbufBytes));  // null the storage of the packets
  bzero(instrOutpktBuf,sizeof(outbufBytes));

  while (!USBConfigured) {
    taskYIELD(); //Wait here until USB hw is configured
  }

  while (TRUE) { // If empty, yield, which gives lower-priority tasks some time
    if ((rval=readPacket(instrInpktBuf))==0) {
      taskYIELD();// no Rx DATA so yield to other threads
    } else {
      instrOutpktBuf->version = 1;
      instrOutpktBuf->cmd = CMD_ACK; // default all packets to ACK-type
      instrOutpktBuf->length = htole16(USB_PKT_MIN_HEADER_SZ); // give all response packets a length      
      switch (instrInpktBuf->cmd) {
      case CMD_ACK:
        break;

      case CMD_NAK:
        break;

      case CMD_ECHO:
        copyPacket(instrInpktBuf, instrOutpktBuf);
        break;

      case CMD_ATT: //Payload is a 16-bit integer representing attenSetting_t enum
        atten = (attenSetting_t)le16toh(instrInpktBuf->payload.pl_uint16.a_uint16);
        setAttenSetting(atten);
        break;

      case CMD_SPDT: //Payload is two bytes: { SW1|SW2, J1|J2 }
        swSel = (spdtSel_t)instrInpktBuf->payload.pl_2uchar.uchar1;
        swState = (spdtSetting_t)instrInpktBuf->payload.pl_2uchar.uchar2;
        spdt_set(swSel, swState);
        break;

      case CMD_SP8T: //Payload is one byte {J1|J2|J3|J4|J5|J6|J7|J8}
        muxSel = (sp8tSel_t)instrInpktBuf->payload.pl_uchar.a_uchar;
        setSP8T(muxSel);
        break;

      case CMD_NOTIFY: //Payload is 1 byte color, 3 unsigned long: onTime, offTime, count
        color = instrInpktBuf->payload.pl_notifyLight.color;
        onTime = le32toh(instrInpktBuf->payload.pl_notifyLight.onTime);
        offTime = le32toh(instrInpktBuf->payload.pl_notifyLight.offTime);
        count = le32toh(instrInpktBuf->payload.pl_notifyLight.count);
        stackNotify(color, onTime, offTime, count);
        break;

      case CMD_WRITEEE: //
        //eeprom9366_erase(instrInpktBuf->payload.pl_eeprom.address);
        eeprom9366_write(instrInpktBuf->payload.pl_eeprom.address,instrInpktBuf->payload.pl_eeprom.data);
        break;

      case CMD_READEE:
        copyPacket(instrInpktBuf, instrOutpktBuf);
        instrOutpktBuf->payload.pl_eeprom.data=eeprom9366_read(instrInpktBuf->payload.pl_eeprom.address);
        break;

      case CMD_ERASEALL:
        eeprom9366_eraseAll();
        break;
        
      default:
        dprintf("ERROR: unrecognized or NYI command: %u\r\n",instrInpktBuf->cmd);
        instrOutpktBuf->cmd = CMD_NAK;
        break;

      } //cmd dispatcher switch
      writePacket(instrOutpktBuf,waitIfFull);
    } // if received a packet
  } // while true
} //instrument task thread

