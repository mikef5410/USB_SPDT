/*******************************************************************************
*           Copyright (C) 2015 Michael R. Ferrara, All rights reserved.
*
*                       Santa Rosa, CA 95404
*                       Tel:(707)536-1330
*
* Filename:     stacklight.c
*
* Description: STACKLIGHT "driver"
*
*******************************************************************************/
//#define TRACE_PRINT 1

#define GLOBAL_STACKLIGHT
#include "stacklight.h"

#include "OSandPlatform.h"

// 30ms is enough to switch the stacklightuator
#define STACKLIGHT_DLY 30

#define STACKRED S2U
#define STACKYEL S4U
#define STACKGRN S3D

void stackRed(int on) {

  hvOn(1);//Charge up the output cap
  
  if (on) {
    gpio_clear(STACKYEL);
    gpio_clear(STACKGRN);
    gpio_set(STACKRED);
  } else {
    gpio_clear(STACKRED);
    hvOn(0);
  }

}

void stackYel(int on) {

  hvOn(1);//Charge up the output cap
  
  if (on) {
    gpio_clear(STACKRED);
    gpio_clear(STACKGRN);
    gpio_set(STACKYEL);
  } else {
    gpio_clear(STACKYEL);
    hvOn(0);
  }

}


void stackGrn(int on) {

  hvOn(1);//Charge up the output cap
  
  if (on) {
    gpio_clear(STACKRED);
    gpio_clear(STACKYEL);
    gpio_set(STACKGRN);
  } else {
    gpio_clear(STACKGRN);
    hvOn(0);
  }

}
