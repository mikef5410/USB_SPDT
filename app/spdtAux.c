/*******************************************************************************
* Copyright (C) 2016 Mike Ferrara (mikef@mrf.sonoma.ca.us), All rights reserved.
*
*
* Filename:     spdtAux.c
*
* Description: SPDTAUX 12V SPDT coax switches controlled by Aux pins (Maple DVT)
* Dow-Key 401Y-421132A 12V w/ TTL controls (Latching)
* Switch S1 is controlled by Aux1 (A,J1-C) and Aux2 (B,J2-C)
* Switch S2 is controlled by Aux3 (A,J1-C) and Aux4 (B,J2-C)
*******************************************************************************/
//#define TRACE_PRINT 1

#include "OSandPlatform.h"

#define GLOBAL_SPDTAUX
#include "spdtAux.h"

#define SW_DELAY 60

//Switch S1
//If on==1, select J1-Common, else select J2-Common
void s1_j1sel(int on) {
  if (on) {
    gpio_clear(AUX2);
    gpio_set(AUX1);
    delayms(SW_DELAY);
    gpio_clear(AUX1); //Latching relays
  } else {
    gpio_clear(AUX1);
    gpio_set(AUX2);
    delayms(SW_DELAY);
    gpio_clear(AUX2); //Latching relays
  }
}

//Switch S2
//If on==1, select J1-Common, else select J2-Common
void s2_j1sel(int on) {
  if (on) {
    gpio_clear(AUX4);
    gpio_set(AUX3);
    delayms(SW_DELAY);
    gpio_clear(AUX3); //Latching relays
  } else {
    gpio_clear(AUX3);
    gpio_set(AUX4);
    delayms(SW_DELAY);
    gpio_clear(AUX4); //Latching relays
  }
}

void spdt_set(spdtSel_t swtch, spdtSetting_t sel) {
  if (swtch==SW1) {
    s1_j1sel(sel==J1SEL);
  } else {
    s2_j1sel(sel==J1SEL);
  }
}
