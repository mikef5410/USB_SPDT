#ifndef _USBCMDIO_INCLUDED
#define _USBCMDIO_INCLUDED

#include "stdint.h"

#ifdef GLOBAL_USBCMDIO
#define USBCMDIOGLOBAL
#define USBCMDIOPRESET(A) = (A)
#else
#define USBCMDIOPRESET(A)
#ifdef __cplusplus
#define USBCMDIOGLOBAL extern "C"
#else
#define USBCMDIOGLOBAL extern
#endif  /*__cplusplus*/
#endif                          /*GLOBAL_USBCMDIO */

#ifdef __cplusplus
extern "C" {
#endif


  // Packet type
  // Making a change here? Make sure you update ../apptest/cmddefs to match ...
  typedef enum {
    CMD_ACK = 0,    // 00 0x00  
    CMD_NAK,        // 01 0x01  
    CMD_RESET,      // 02 0x02  ver 2.0 of protocol: supports 8-bit parameter (hard, soft, etc.)
    CMD_ID,         // 03 0x03  
    CMD_ECHO,       // 04 0x04  
    CMD_SSN,        // 05 0x05  silicon serial number
    CMD_DIAG,       // 06 0x06  diagnostic self-test cmd, result-string
    CMD_SP8T,       // 07 0x07  set the SP8T to selected (payload one byte)
    CMD_AUXOUT,     // 08 0x08  set auxout bits (payload one byte)
    CMD_AUXIN,      // 09 0x09  read aux in bits
    CMD_ATT,        // 10 0x0A  set attenuation (one byte, enum)
    CMD_LIGHT,      // 11 0x0B  set stacklight static (one byte, bitmapped)
    CMD_NOTIFY,     // 12 0x0C  blink light (4 bytes: R|Y|G|OFF, on time, off time, count)
    CMD_READEE,     // 13 0x0D  read one byte of EEprom, 2-byte address (LE)
    CMD_WRITEEE,    // 14 0x0E  write one byte of EEprom, 2-byte address (LE), 1 byte data
  } pkttype_t;

  typedef enum {
    ATT_0DB = 0,
    ATT_10DB,
    ATT_20DB,
    ATT_30DB,
    ATT_40DB,
    ATT_50DB,
    ATT_60DB,
    ATT_70DB
  } attenSetting_t;
    
  typedef enum {
    FLOW_USB = 0,
    FLOW_KBD,
    FLOW_DEBUGSHELL,
  } cmdflow_t;
  
  // define some generic payloads for parameterized commands
  //   For example, RESET now takes an argument for type of reset
  typedef struct __attribute__((__packed__)) {
    int8_t a_bool;
    // padding/alignment ???
  } payload_bool_t;

  typedef struct __attribute__((__packed__)) {
    int8_t a_char;
  } payload_char_t;

  typedef struct __attribute__((__packed__)) {
    int16_t a_int16;
  } payload_int16_t;

  typedef struct __attribute__((__packed__)) {
    int32_t a_int32;
  } payload_int32_t;

  typedef struct __attribute__((__packed__)) {
    uint8_t a_uchar;
  } payload_uchar_t;

  typedef struct __attribute__((__packed__)) {
    uint16_t a_uint16;
  } payload_uint16_t;

  typedef struct __attribute__((__packed__))  {
    uint32_t a_u32;
  } payload_uint32_t;

  typedef struct __attribute__((__packed__))  {
    uint8_t  uchar1;
    uint8_t  uchar2;
  } payload_2uchar_t;

  typedef struct  __attribute__((__packed__)) {
    uint16_t uint1;
    uint16_t uint2;
  } payload_2uint16_t;

  typedef struct  __attribute__((__packed__)) {
    uint16_t uint1;
    uint16_t uint2;
    uint16_t uint3;
  } payload_3uint16_t;

  typedef struct {            // size description
    uint8_t  productID;       // 1    start at 1
    uint8_t  protocolVersion; // 1    Must be 0x01 !!
    uint8_t  fwRev_major;     // 1    start at 00
    uint8_t  fwRev_minor;     // 1    start at 01
    uint16_t fwRev_build;     // 2    derive from Jenkins, start at 104
    //---
    // 6    size to this point
    //
    uint8_t  bld_sha_len;     // 1    value = 8: 7 for SHA1, 1 for null-term
    uint8_t  bld_sha[8];      // 15   size to this point
    uint8_t  bld_info_len;    // 1    set dynamically
    uint8_t  bld_info [220];  // up to 220 chars, null-term
  } payload_id_response_t;

  typedef struct {        // SSN: silicon serial number
    uint32_t ssn_values[3]; // zero-fill unused bits, little-endian
  } payload_ssn_t;

  //Packet has a flow indicator of type cmdflow_t but a packet on the wire doesn't have this field.
  //It's added by the receiver ...
  // Max packet size is 64 bytes on wire, payload is 58 bytes
  typedef struct  __attribute__((__packed__)) {
    uint8_t  flow;
    uint8_t  version;   // Protocol version
    uint16_t length;    // Number of bytes in packet
    uint8_t  cmd;      // one byte
    uint16_t checksum;  // two bytes computed with checksum field=0
    union {
      uint8_t               asBytes[58];
      payload_id_response_t id_resp;
      payload_ssn_t         pl_ssn;  //  ssn_resp;
      payload_bool_t        pl_bool;
      payload_char_t        pl_char;   // set/get 1 c
      payload_uchar_t       pl_uchar;  // set/get 1 uchar
      payload_int16_t       pl_int16;  // set/get 1 int16 ...
      payload_uint16_t      pl_uint16; // set/get 1 uint16 ..
      payload_int32_t       pl_int32;
      payload_uint32_t      pl_uint32;
    } payload;
  } cmd_packet_t;

#define USB_PKT_MIN_HEADER_SZ (6)
  
  // Simple, speedy 8 bit checksum
  typedef struct __attribute__((__packed__)) {
    uint16_t Checksum1;
    uint16_t Checksum2;
  } FLETCHER_CHECKSUM;

  /* Compute fletcher checksum in a streaming fashion */
#ifndef __GNUC__
#define FLETCH(SUM,BYTE) (SUM)->Checksum1 += (BYTE);    \
  if ((SUM)->Checksum1 > 254) (SUM)->Checksum1 -= 255;  \
  (SUM)->Checksum2 += (SUM)->Checksum1;                 \
  if ((SUM)->Checksum2 > 254) (SUM)->Checksum2 -= 255;
#else
  static inline void FLETCH(FLETCHER_CHECKSUM *checksums, uint8_t byte) {
    checksums->Checksum1 += byte;
    if (checksums->Checksum1 > 254) checksums->Checksum1 -= 255;
    checksums->Checksum2 += checksums->Checksum1;
    if (checksums->Checksum2 > 254) checksums->Checksum2 -= 255;
    return;
  }
#endif
  
#ifdef __cplusplus
}
#endif


#endif                          //_USBCMDIO_INCLUDED
  
