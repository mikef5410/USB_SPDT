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

static int cmd_sw(int argc, char **argv)
{
  uint32_t switchN = 0;
  uint32_t val = 0;

  if (argc == 3) {
    switchN = strtoul(argv[1],NULL,0);
    val = strtoul(argv[2],NULL,0);
    switch(switchN) {
    case 1: s1(val); break;
    case 2: s2(val); break;
    case 3: s3(val); break;
    case 4: s4(val); break;
    default: myprintf("Switch can be 1,2,3,or 4\n");
      return(1);
    }
  } else {
    myprintf("Usage: sw <switch number> 1|0\n");
    return(1);
  }
  return(0);
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
    case 4: aux4(val); break;
    default: myprintf("Aux can be 0,1,2,3,or 4\n");
      return(1);
    }
  } else {
    myprintf("Usage: aux <switch number> 1|0\n");
    return(1);
  }
  return(0);
}

static int cmd_buzz(int argc, char **argv)
{
  uint32_t switchN = 0;
  int n = 100000;
  uint32_t on=0;
  const uint32_t LDLY=5;
  
  switchN = strtoul(argv[1],NULL,0);
  if (argc==3) {
    n = strtoul(argv[2],NULL,0);
  }
  
  switch(switchN) {
  case 1:
    for (int k=0; k<n; k++) {
      on = !on;
      s1(on);
      delayms(LDLY);
    }
    break;
  case 2:
    for (int k=0; k<n; k++) {
      on = !on;
      s2(on);
      delayms(LDLY);
    }
    break;
  case 3:
    for (int k=0; k<n; k++) {
      on = !on;
      s3(on);
      delayms(LDLY);
    }
    break;
  case 4:
    for (int k=0; k<n; k++) {
      on = !on;
      s4(on);
      delayms(LDLY);
    }
    break;
  default:
    myprintf("Bad args.\n");
    return(1);
  }
  return(0);
}

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
  {"","sw",               "switchnum 1|0         Turn switch/step on/off", cmd_sw, NULL},
  {"","aux",              "auxnum 1|0            Turn aux bit on/off", cmd_aux, NULL},
  {"","buzz",             "switchnum n           Turn switch on/off n times", cmd_buzz, NULL},
    //LAST ENTRY
  {NULL, NULL, NULL, NULL, NULL}
};


// Add your command table here ... most general last
dispatchEntry* dispatchTableTable[] = {
    &(mainCommands[0]),     //command.c
    &(commonCommands[0]),  //debug_shell.c
    NULL
};
