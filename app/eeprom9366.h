/*******************************************************************************
*           Copyright (C) 2015 Michael R. Ferrara, All rights reserved.
*
*                       Santa Rosa, CA 95404
*                       Tel:(707)536-1330
*
* Filename:     eeprom9366.h
*
* Description: EEPROM9366 "driver"
*
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _EEPROM9366_INCLUDED
#define _EEPROM9366_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_EEPROM9366
#define EEPROM9366GLOBAL
#define EEPROM9366PRESET(A) = (A)
#else
#define EEPROM9366PRESET(A)
#ifdef __cplusplus
#define EEPROM9366GLOBAL extern "C"
#else
#define EEPROM9366GLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_EEPROM9366 */

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------



// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------

EEPROM9366GLOBAL void eeprom9366_init(void);
EEPROM9366GLOBAL void eeprom9366_eraseAll(void);
EEPROM9366GLOBAL void eeprom9366_erase(uint8_t address);
EEPROM9366GLOBAL void eeprom9366_write(uint8_t address, uint8_t data);
EEPROM9366GLOBAL uint8_t eeprom9366_read(uint8_t address);

#ifdef TESTEEPROM
EEPROM9366GLOBAL void eeprom9366_test(void);
#endif

#endif				//_EEPROM9366_INCLUDED
