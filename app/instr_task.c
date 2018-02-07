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
#define GLOBAL_VERSION
#define GLOBAL_INSTR_TASK
#include "instr_task.h"

#include "OSandPlatform.h"

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

// NOTE: most of the ID is still hard-coded
// get fw id:  interrogate version.h structure for ID info
//             this function is hard coded to:
//             major.minor.build =  00.01.<build count>
//
// The SHA1 is an auto-build value scripted in version.mk
// It is part of the changeset array, which consists of build number,
// followed by an underscore, then 7 digits of SHA1, or + and six digits
// of the SHA1 if the repo was built with any changed file (dirty repo)
#define SZ_BUILD_CNT_ATOI 4      // size of scratch-pad for atoi call
static payload_id_response_t myID;

// #define SZ_BUILD_SHA1     7   // version.mk generates this !!
void get_instrument_ID(cmd_packet_t *buffer) {
  char *pSha1    = NULL;
  //  char *pBldInfo = NULL;
  char *pChars   = NULL;
  char bldStr[8];    // scratch-pad for atoi: converts build-num to integer
  //char bldInfo[41]; // the other build-info
  int k;

  memset(&myID,   0,sizeof(myID));
  memset(&bldStr, 0,sizeof(bldStr));  // NULL atoi conversion array
  //memset(&bldInfo,0,sizeof(bldInfo)); // NULL array
  myID.productID       = shortProdID();      // start at 1, max 255
  myID.protocolVersion = 1;      // increment when the ID pkt format changes
  myID.fwRev_major     = 1;      // range: 0-99
  myID.fwRev_minor     = 0;     // range: 0-99

  // copy the version.h build number (changeset) to a small array
  //      for conversion to an integer (atoi)
  pSha1  = (char *)&build_sha1[0];    // first 4 chars: 0092_+ae431f
  pChars = (char *)&bldStr[0];        // point to scratch-pad for atoi
  for (k=0;k<SZ_BUILD_CNT_ATOI;k++) { // copy build-count from changeset
    *pChars++ = *pSha1++;
  }
  myID.fwRev_build = (uint16_t) atoi(&bldStr[0]);
  //pSha1++;  // move past underscore
  pSha1--;
  
  // copy the SHA1 from the changeset
  pChars = (char *)&myID.bld_sha[0];
  for (k=0;k<SZ_BUILD_SHA1;k++) {         // copy SHA1 from changeset
	*pChars++ = *pSha1++;
  }
  *pChars++ = 0x00;  // null-terminate
  myID.bld_sha_len = SZ_BUILD_SHA1+1;  // null-terminate-len

#if 0
  pBldInfo = (char *)&build_info[0];
  pChars   = (char *)&myID.bld_info[0];
  for (k=0;k<MIN(strlen(build_info),40);k++) {         // copy SHA1 from changeset
	*pChars++ = *pBldInfo++;
  }
  *pChars++ = 0x00;  // null-terminate
  myID.bld_info_len = MIN(strlen(build_info),40) + 1;
#endif
  // copy myID into the payload
  buffer->payload.id_resp = (payload_id_response_t)myID;
  buffer->length = USB_PKT_MIN_HEADER_SZ + 16; //+ myID.bld_info_len; 
} // end get_instrument_ID


__attribute__((noreturn)) portTASK_FUNCTION(vInstrumentTask, pvParameters) {

  (void)pvParameters;
  int32_t rval = 0;
  portTickType waitIfFull = 10;
  spdtSel_t swSel;
  spdtSetting_t swState;

  
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

      case CMD_ID:
        copyPacket(instrInpktBuf, instrOutpktBuf);
        get_instrument_ID(instrOutpktBuf);
        break;

      case CMD_ECHO:
        copyPacket(instrInpktBuf, instrOutpktBuf);
        break;

      case CMD_SPDT: //Payload is two bytes: { SW1|SW2, J1|J2 }
        swSel = (spdtSel_t)instrInpktBuf->payload.pl_2uchar.uchar1;
        swState = (spdtSetting_t)instrInpktBuf->payload.pl_2uchar.uchar2;
        spdt_set(swSel, swState);
        break;

      case CMD_WRITEEE: //
        //eeprom9366_erase(instrInpktBuf->payload.pl_eeprom.address);
        eeprom9366_write(instrInpktBuf->payload.pl_eeprom.address,instrInpktBuf->payload.pl_eeprom.data);
        break;

      case CMD_READEE:
        copyPacket(instrInpktBuf, instrOutpktBuf);
        instrOutpktBuf->payload.pl_eeprom.data=eeprom9366_read(instrInpktBuf->payload.pl_eeprom.address);
        instrOutpktBuf->length = htole16(USB_PKT_MIN_HEADER_SZ + 3);
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

