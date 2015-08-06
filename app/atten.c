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

#define GLOBAL_ATTEN
#include "atten.h"

#include "OSandPlatform.h"

void s1(int up) {

  hvOn(1);
  delayms(20);

  if (up) {
    gpio_clear(S1D);
    gpio_set(S1U);
  } else {
    gpio_clear(S1U);
    gpio_set(S1D);
  }

  delayms(50);
  gpio_clear(S1U);
  gpio_clear(S1D);
}

void s2(int up) {

  hvOn(1);
  delayms(20);

  if (up) {
    gpio_clear(S2D);
    gpio_set(S2U);
  } else {
    gpio_clear(S2U);
    gpio_set(S2D);
  }

  delayms(50);
  gpio_clear(S2U);
  gpio_clear(S2D);
}

void s3(int up) {

  hvOn(1);
  delayms(20);

  if (up) {
    gpio_clear(S3D);
    gpio_set(S3U);
  } else {
    gpio_clear(S3U);
    gpio_set(S3D);
  }

  delayms(50);
  gpio_clear(S3U);
  gpio_clear(S3D);
}

void s4(int up) {

  hvOn(1);
  delayms(20);

  if (up) {
    gpio_clear(S4D);
    gpio_set(S4U);
  } else {
    gpio_clear(S4U);
    gpio_set(S4D);
  }

  delayms(50);
  gpio_clear(S4U);
  gpio_clear(S4D);
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

