/*******************************************************************************
* Copyright (C) 2016 Mike Ferrara (mikef@mrf.sonoma.ca.us), All rights reserved.
*
*
* Filename:     spdt.c
*
* Description: SPDT 12V SPDT coax switches 
* Dow-Key 401Y-421132A 12V w/ TTL controls (Latching)
* Switch S1 is controlled by Aux1 (A,J1-C) and Aux2 (B,J2-C)
* Switch S2 is controlled by Aux3 (A,J1-C) and Aux4 (B,J2-C)
*******************************************************************************/
//#define TRACE_PRINT 1

#include "OSandPlatform.h"

#define GLOBAL_SPDT
#include "spdt.h"

#define SW_DELAY 60
static uint8_t s1_pulseHigh=1; //1 Means common on the switch is GND
static uint8_t s2_pulseHigh=1; //0 Means common on the switch is +12

void s1_setPulseHigh(int setting)
{
  gpio_clear(SWEnable);
  s1_pulseHigh=setting;
  if (s1_pulseHigh) { //Set Idle value
    gpio_clear(S1D);
    gpio_clear(S1U);
  } else {
    gpio_set(S1D);
    gpio_set(S1U);
  }
}
void s2_setPulseHigh(int setting)
{
  gpio_clear(SWEnable);
  s2_pulseHigh=setting;
  if (s2_pulseHigh) { //Set Idle value
    gpio_clear(S2D);
    gpio_clear(S2U);
  } else {
    gpio_set(S2D);
    gpio_set(S2U);
  }
}

//Switch S1
//If on==1, select J1-Common, else select J2-Common
void s1_j1sel(int on)
{
  hvOn(1);
  gpio_set(SWEnable);
  if (s1_pulseHigh) {
    if (on) {
      gpio_clear(S1D);
      gpio_set(S1U);
      delayms(SW_DELAY);
      gpio_clear(S1U);		//Latching relays
    } else {
      gpio_clear(S1U);
      gpio_set(S1D);
      delayms(SW_DELAY);
      gpio_clear(S1D);		//Latching relays
    }
  } else {
    if (on) {
      gpio_set(S1D);
      gpio_clear(S1U);
      delayms(SW_DELAY);
      gpio_set(S1U);		//Latching relays
    } else {
      gpio_set(S1U);
      gpio_clear(S1D);
      delayms(SW_DELAY);
      gpio_set(S1D);		//Latching relays
    }
  }
  gpio_clear(SWEnable);
}

//Switch S2
//If on==1, select J1-Common, else select J2-Common
void s2_j1sel(int on)
{
  hvOn(1);
  gpio_set(SWEnable);
  if (s2_pulseHigh) {
    if (on) {
      gpio_clear(S2D);
      gpio_set(S2U);
      delayms(SW_DELAY);
      gpio_clear(S2U);		//Latching relays
    } else {
      gpio_clear(S2U);
      gpio_set(S2D);
      delayms(SW_DELAY);
      gpio_clear(S2D);		//Latching relays
    }
  } else {
    if (on) {
      gpio_set(S2D);
      gpio_clear(S2U);
      delayms(SW_DELAY);
      gpio_set(S2U);		//Latching relays
    } else {
      gpio_set(S2U);
      gpio_clear(S2D);
      delayms(SW_DELAY);
      gpio_set(S2D);		//Latching relays
    }
  }
  gpio_clear(SWEnable);
}

void spdt_set(spdtSel_t swtch, spdtSetting_t sel)
{
  if (swtch == SW1) {
    s1_j1sel(sel == J1SEL);
  } else {
    s2_j1sel(sel == J1SEL);
  }
}


void aux0(int on)
{
  if (on) {
    gpio_set(AUX0);
  } else {
    gpio_clear(AUX0);
  }
}

void aux1(int on)
{
  if (on) {
    gpio_set(AUX1);
  } else {
    gpio_clear(AUX1);
  }
}


void aux2(int on)
{
  if (on) {
    gpio_set(AUX2);
  } else {
    gpio_clear(AUX2);
  }
}


void aux3(int on)
{
  if (on) {
    gpio_set(AUX3);
  } else {
    gpio_clear(AUX3);
  }
}
