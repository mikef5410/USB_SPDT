/*******************************************************************************
* Copyright (C) 2016 Mike Ferrara (mikef@mrf.sonoma.ca.us), All rights reserved.
*
*
* Filename:     instr_task.h
*
* Description: INSTR_TASK  The author neglected to write a description.
*
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _INSTR_TASK_INCLUDED
#define _INSTR_TASK_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_INSTR_TASK
#define INSTR_TASKGLOBAL
#define INSTR_TASKPRESET(A) = (A)
#else
#define INSTR_TASKPRESET(A)
#ifdef __cplusplus
#define INSTR_TASKGLOBAL extern "C"
#else
#define INSTR_TASKGLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_INSTR_TASK */

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------

#endif				//_INSTR_TASK_INCLUDED
