/*******************************************************************************
* Copyright (C) 2016 Mike Ferrara (mikef@mrf.sonoma.ca.us), All rights reserved.
*
*
* Filename:     sp8t.h
*
* Description: SP8T  The author neglected to write a description.
*
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _SP8T_INCLUDED
#define _SP8T_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_SP8T
#define SP8TGLOBAL
#define SP8TPRESET(A) = (A)
#else
#define SP8TPRESET(A)
#ifdef __cplusplus
#define SP8TGLOBAL extern "C"
#else
#define SP8TGLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_SP8T */

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------
SP8TGLOBAL void setSP8T(sp8tSel_t sel);
#endif				//_SP8T_INCLUDED
