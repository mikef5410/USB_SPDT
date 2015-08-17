/*******************************************************************************
*           Copyright (C) 2015 Michael R. Ferrara, All rights reserved.
*
*                       Santa Rosa, CA 95404
*                       Tel:(707)536-1330
*
* Filename:     stacklight.h
*
* Description: STACKLIGHT "driver"
*
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _STACKLIGHT_INCLUDED
#define _STACKLIGHT_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_STACKLIGHT
#define STACKLIGHTGLOBAL
#define STACKLIGHTPRESET(A) = (A)
#else
#define STACKLIGHTPRESET(A)
#ifdef __cplusplus
#define STACKLIGHTGLOBAL extern "C"
#else
#define STACKLIGHTGLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_STACKLIGHT */

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------
#define UP 1
#define DOWN 0
#define ON 1
#define OFF 0

#define R 1
#define Y 2
#define G 3

STACKLIGHTGLOBAL void stackRed(int on);
STACKLIGHTGLOBAL void stackYel(int on);
STACKLIGHTGLOBAL void stackGrn(int on);
STACKLIGHTGLOBAL uint32_t stackNotify(uint32_t col, uint32_t on, uint32_t off, uint32_t cnt);
#endif				//_STACKLIGHT_INCLUDED
