#include "OSandPlatform.h"
#include "debug_shell.h"

#define GLOBAL_VERSION
#include "version.h"

#ifndef COUNTOF
#define COUNTOF(A) (sizeof(A)/sizeof(A[0]))
#endif


#ifdef BUILD_INFO
static int cmd_buildInfo(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  myprintf("%s\r\n", build_info);
  return (0);
}
#endif

#ifdef BUILD_SHA1
static int cmd_build_sha1(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  myprintf("%s, %s\r\n", build_sha1, build_sha1_full);
  return (0);
}
#endif

static int cmd_hardfault(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  TRIG_HARDFAULT; //trigger a hard fault
  return(0);
}

static int cmd_timer(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  for (int j=0; j<3; j++) {
    gpio_toggle(GPIOC, GPIO7);
    volatile uint64_t start=hiresTimer_getTime();
    vTaskDelay(10/portTICK_RATE_MS);
    volatile int64_t delta = hiresTimer_getTime() - start;
    //gpio_clear(GPIOC, GPIO7);
    myprintf(" 10 ms = %d us \n", (int)tics2us(delta));
  }
  return(0);
}

static int cmd_hv(int argc, char **argv)
{
  uint16_t val=0;
  if (argc == 1) {
    val=gpio_get(GPIOD,GPIO2);
    myprintf("HV is %s\n",val?"ON":"OFF");
    return(0);
  }
  if (argc == 2) {
    val = (uint16_t)strtoul(argv[1],NULL,0);
    hvOn(val!=0);
    return(0);
  }
  myprintf("usage: hv 1|0 \n");
  return(1);
}

static int cmd_sw1(int argc, char **argv)
{
  if (argc == 2) {
    if (*argv[1] == 'A' || *argv[1] == 'a') {
      s1_j1sel(1);
      return(0);
    }
    if (*argv[1] == 'B' || *argv[1] == 'b') {
      s1_j1sel(0);
      return(0);
    }
  }
  myprintf("usage: s1 A|B \n");
  return(1);
}

static int cmd_sw2(int argc, char **argv)
{
  if (argc == 2) {
    if (*argv[1] == 'A' || *argv[1] == 'a') {
      s2_j1sel(1);
      return(0);
    }
    if (*argv[1] == 'B' || *argv[1] == 'b') {
      s2_j1sel(0);
      return(0);
    }
  }
  myprintf("usage: s2 A|B \n");
  return(1);
}

static int cmd_aux(int argc, char **argv)
{
  uint32_t switchN = 0;
  uint32_t val = 0;

  if (argc == 3) {
    switchN = strtoul(argv[1],NULL,0);
    val = strtoul(argv[2],NULL,0);
    switch(switchN) {
    case 0: aux0(val); break;
    case 1: aux1(val); break;
    case 2: aux2(val); break;
    case 3: aux3(val); break;
    default: myprintf("Aux can be 0,1,2,3,or 4\n");
      return(1);
    }
  } else {
    myprintf("Usage: aux <switch number> 1|0\n");
    return(1);
  }
  return(0);
}

static int cmd_testee(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  eeprom9366_test();
  return(0);
}

#ifdef TESTEEPROM
static int cmd_eraseee(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  eeprom9366_eraseAll();
  return(0);
}
#endif

dispatchEntry mainCommands[] = {
//Context, Command,        ShortHelp,                                          command proc,  help proc
#ifdef BUILD_INFO
  {"","buildInfo",        "                      Show build info", cmd_buildInfo, NULL},
#endif
#ifdef BUILD_SHA1
  {"","build_sha1",       "                      Show SHA1 info", cmd_build_sha1, NULL},
#endif
  {"","hardfault",        "                      Cause a hard fault", cmd_hardfault, NULL},
  {"","timer",            "                      Test the hires timer", cmd_timer, NULL},
  {"","hv",               "[1|0]                 Turn high voltage on/off", cmd_hv, NULL},
  {"","aux",              "auxnum 1|0            Turn aux bit on/off", cmd_aux, NULL},
  {"","s1",               "[A|B                  Turn switch 1 to A or B", cmd_sw1, NULL},
  {"","s2",               "[A|B                  Turn switch 2 to A or B", cmd_sw2, NULL},
  
#ifdef TESTEEPROM
  {"","testee",           "                      Test eeprom", cmd_testee, NULL},
  {"","eraseee",          "                      Erase entire eeprom", cmd_eraseee, NULL},
#endif
    //LAST ENTRY
  {NULL, NULL, NULL, NULL, NULL}
};


// Add your command table here ... most general last
dispatchEntry* dispatchTableTable[] = {
    &(mainCommands[0]),     //command.c
    &(commonCommands[0]),  //debug_shell.c
    NULL
};
