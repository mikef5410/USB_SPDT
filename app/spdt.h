/*******************************************************************************
* Copyright (C) 2016 Mike Ferrara (mikef@mrf.sonoma.ca.us), All rights reserved.
*
*
* Filename:     spdt.h
*
* Description: SPDT  The author neglected to write a description.
*
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _SPDT_INCLUDED
#define _SPDT_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_SPDT
#define SPDTGLOBAL
#define SPDTPRESET(A) = (A)
#else
#define SPDTPRESET(A)
#ifdef __cplusplus
#define SPDTGLOBAL extern "C"
#else
#define SPDTGLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_SPDT */

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------
SPDTGLOBAL void s1_setPulseHigh(int setting);
SPDTGLOBAL void s2_setPulseHigh(int setting);
SPDTGLOBAL void s1_j1sel(int on);
SPDTGLOBAL void s2_j1sel(int on);
SPDTGLOBAL void spdt_set(spdtSel_t swtch, spdtSetting_t sel);

SPDTGLOBAL void aux0(int on);
SPDTGLOBAL void aux1(int on);
SPDTGLOBAL void aux2(int on);
SPDTGLOBAL void aux3(int on);
SPDTGLOBAL void aux4(int on);
#endif				//_SPDT_INCLUDED
