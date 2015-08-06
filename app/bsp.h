/*******************************************************************************
*           Copyright (C) 2015 Michael R. Ferrara, All rights reserved.
*
*                       Santa Rosa, CA 95404
*                       Tel:(707)536-1330
*
* Filename:     bsp.h
*
* Description: BSP "driver"
*
*******************************************************************************/
//#define TRACE_PRINT 1

#ifndef _BSP_INCLUDED
#define _BSP_INCLUDED

#include "OSandPlatform.h"

#ifdef GLOBAL_BSP
#define BSPGLOBAL
#define BSPPRESET(A) = A
#else
#define BSPPRESET(A)
#ifdef __cplusplus
#define BSPGLOBAL extern "C"
#else
#define BSPGLOBAL extern
#endif	/*__cplusplus*/
#endif				/*GLOBAL_BSP */

// ----------------------------------------------------------------
// PRIVATE API AND SUBJECT TO CHANGE!
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// PUBLIC API definition
// ----------------------------------------------------------------

BSPGLOBAL uint32_t SystemCoreClock;

BSPGLOBAL void greenOn(int on);
BSPGLOBAL void redOn(int on);
BSPGLOBAL void hvOn(int on);
BSPGLOBAL void setupClocks(void);
BSPGLOBAL void setupGPIOs(void);
BSPGLOBAL void setupNVIC(void);
BSPGLOBAL void Delay(volatile uint32_t nCount);

#define BSPGPIO(BANK,NUM) GPIO ## BANK,GPIO ## NUM
#define S1U BSPGPIO(C,0)
#define S1D BSPGPIO(C,1)

#define S2U BSPGPIO(C,2)
#define S2D BSPGPIO(C,3)

#define S3U BSPGPIO(C,4)
#define S3D BSPGPIO(C,5)

#define S4U BSPGPIO(C,6)
#define S4D BSPGPIO(C,7)

#define AUX0 BSPGPIO(C,8)
#define AUX1 BSPGPIO(C,9)
#define AUX2 BSPGPIO(C,10)
#define AUX3 BSPGPIO(C,11)
#define AUX4 BSPGPIO(C,12)

#define HVEnable BSPGPIO(D,2)

#define MSleep(x) Delay((x) * 96000UL)

#endif				//_BSP_INCLUDED
