/*******************************************************************************
* Copyright (C) 2016 Mike Ferrara (mikef@mrf.sonoma.ca.us), All rights reserved.
*
*
* Filename:     spdtAux.h
*
* Description: SPDTAUX  The author neglected to write a description.
*
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _SPDTAUX_INCLUDED
#define _SPDTAUX_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_SPDTAUX
#define SPDTAUXGLOBAL
#define SPDTAUXPRESET(A) = (A)
#else
#define SPDTAUXPRESET(A)
#ifdef __cplusplus
#define SPDTAUXGLOBAL extern "C"
#else
#define SPDTAUXGLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_SPDTAUX */

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------
SPDTAUXGLOBAL void s1_j1sel(int on);
SPDTAUXGLOBAL void s2_j1sel(int on);
SPDTAUXGLOBAL void spdt_set(spdtSel_t swtch, spdtSetting_t sel);

#endif				//_SPDTAUX_INCLUDED
