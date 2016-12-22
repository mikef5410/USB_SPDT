/*******************************************************************************
* Copyright (C) 2016 Mike Ferrara (mikef@mrf.sonoma.ca.us), All rights reserved.
*
*
* Filename:     sp8t.c
*
* Description: SP8T Control SP8T 12V coax switches by S1U/D thru S4U/D (Maple DVT)
* Dow-Key 581J-420823A 12V w/TTL controls (Latching)
*
*******************************************************************************/
//#define TRACE_PRINT 1

#include "OSandPlatform.h"

#define GLOBAL_SP8T
#include "sp8t.h"

#define SP8T_DELAY 200

void setSP8T(sp8tSel_t sel) {
  gpio_clear(S1U);
  gpio_clear(S2U);
  gpio_clear(S3U);
  gpio_clear(S4U);
  gpio_clear(S1D);
  gpio_clear(S2D);
  gpio_clear(S3D);
  gpio_clear(S4D);
  switch (sel) {
  case J1:
    myprintf("J1\n");
    gpio_set(S1U);
    delayms(SP8T_DELAY);
    gpio_clear(S1U);
    break;
  case J2:
    myprintf("J2\n");
    gpio_set(S2U);
    delayms(SP8T_DELAY);
    gpio_clear(S2U);
    break;
  case J3:
    myprintf("J3\n");
    gpio_set(S3U);
    delayms(SP8T_DELAY);
    gpio_clear(S3U);
    break;
  case J4:
    gpio_set(S4U);
    delayms(SP8T_DELAY);
    gpio_clear(S4U);
    break;
  case J5:
    gpio_set(S1D);
    delayms(SP8T_DELAY);
    gpio_clear(S1D);
    break;
  case J6:
    gpio_set(S2D);
    delayms(SP8T_DELAY);
    gpio_clear(S2D);
    break;
  case J7:
    gpio_set(S3D);
    delayms(SP8T_DELAY);
    gpio_clear(S3D);
    break;
  case J8:
    gpio_set(S4D);
    delayms(SP8T_DELAY);
    gpio_clear(S4D);
    break;
  default:
    ;
  }
}
