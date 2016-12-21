/*******************************************************************************
*           Copyright (C) 2015 Michael R. Ferrara, All rights reserved.
*
*                       Santa Rosa, CA 95404
*                       Tel:(707)536-1330
*
* Filename:     atten.c
*
* Description: ATTEN "driver"
*
*******************************************************************************/
//#define TRACE_PRINT 1
#include "OSandPlatform.h"
#include "usbcmdio.h"

#define GLOBAL_ATTEN
#include "atten.h"


//Atten is 10,20,20,20
//UP adds attenuation. DOWN removes it
//   dB           0  10  20  30  40  50  60  70
static int s1_att[8] = { 0,  1,  0,  1,  0,  1,  0,  1 };
static int s2_att[8] = { 0,  0,  0,  0,  1,  1,  1,  1 };                           
static int s3_att[8] = { 0,  0,  0,  0,  0,  0,  1,  1 };
static int s4_att[8] = { 0,  0,  1,  1,  1,  1,  1,  1 };


// 30ms is enough to switch the attenuator
#define ATTEN_DLY 60


void s1(int up) {

  hvOn(1);//Charge up the output cap
  
  if (up) {
    gpio_clear(S1D);
    gpio_set(S1U);
  } else {
    gpio_clear(S1U);
    gpio_set(S1D);
  }

  delayms(ATTEN_DLY);
  gpio_clear(S1U);
  gpio_clear(S1D);
  hvOn(0);//Supply stays charged up enough to switch the attenuator
}

void s2(int up) {

  hvOn(1);

  if (up) {
    gpio_clear(S2D);
    gpio_set(S2U);
  } else {
    gpio_clear(S2U);
    gpio_set(S2D);
  }

  delayms(ATTEN_DLY);
  gpio_clear(S2U);
  gpio_clear(S2D);
  hvOn(0);
}

void s3(int up) {

  hvOn(1);

  if (up) {
    gpio_clear(S3D);
    gpio_set(S3U);
  } else {
    gpio_clear(S3U);
    gpio_set(S3D);
  }

  delayms(ATTEN_DLY);
  gpio_clear(S3U);
  gpio_clear(S3D);
  hvOn(0);
}

void s4(int up) {
  hvOn(1);
  
  if (up) {
    gpio_clear(S4D);
    gpio_set(S4U);
  } else {
    gpio_clear(S4U);
    gpio_set(S4D);
  }

  delayms(ATTEN_DLY);
  gpio_clear(S4U);
  gpio_clear(S4D);
  hvOn(0); 
}

void setAtten(int attenuation) {
  attenuation = (int)(attenuation/10);
  if ((attenuation < 0) || (attenuation>7)) return;

  s1(s1_att[attenuation]==0);
  s2(s2_att[attenuation]==0);
  s3(s3_att[attenuation]==0);
  s4(s4_att[attenuation]==0);
  return;
}

void setAttenSetting(attenSetting_t att) {
  int attenIndex=0;
  
  switch (att) {
  case ATT_0DB:
    attenIndex=0;
    break;
  case ATT_10DB:
    attenIndex=1;
    break;
  case ATT_20DB:
    attenIndex=2;
    break;
  case ATT_30DB:
    attenIndex=3;
    break;
  case ATT_40DB:
    attenIndex=4;
    break;
  case ATT_50DB:
    attenIndex=5;
    break;
  case ATT_60DB:
    attenIndex=6;
    break;
  case ATT_70DB:
    attenIndex=7;
    break;
  default:
    attenIndex=7;
  }

  s1(s1_att[attenIndex]==0);
  s2(s2_att[attenIndex]==0);
  s3(s3_att[attenIndex]==0);
  s4(s4_att[attenIndex]==0);
  return;
}

void aux0(int on) {
  if (on) {
    gpio_set(AUX0);
  } else {
    gpio_clear(AUX0);
  }
}

void aux1(int on) {
  if (on) {
    gpio_set(AUX1);
  } else {
    gpio_clear(AUX1);
  }
}


void aux2(int on) {
  if (on) {
    gpio_set(AUX2);
  } else {
    gpio_clear(AUX2);
  }
}


void aux3(int on) {
  if (on) {
    gpio_set(AUX3);
  } else {
    gpio_clear(AUX3);
  }
}


void aux4(int on) {
  if (on) {
    gpio_set(AUX4);
  } else {
    gpio_clear(AUX4);
  }
}

