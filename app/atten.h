/*******************************************************************************
*           Copyright (C) 2015 Michael R. Ferrara, All rights reserved.
*
*                       Santa Rosa, CA 95404
*                       Tel:(707)536-1330
*
* Filename:     atten.h
*
* Description: ATTEN "driver"
*
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _ATTEN_INCLUDED
#define _ATTEN_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_ATTEN
#define ATTENGLOBAL
#define ATTENPRESET(A) = (A)
#else
#define ATTENPRESET(A)
#ifdef __cplusplus
#define ATTENGLOBAL extern "C"
#else
#define ATTENGLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_ATTEN */

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

ATTENGLOBAL void s1(int up);
ATTENGLOBAL void s2(int up);
ATTENGLOBAL void s3(int up);
ATTENGLOBAL void s4(int up);

ATTENGLOBAL void aux0(int on);
ATTENGLOBAL void aux1(int on);
ATTENGLOBAL void aux2(int on);
ATTENGLOBAL void aux3(int on);
ATTENGLOBAL void aux4(int on);
#endif				//_ATTEN_INCLUDED
