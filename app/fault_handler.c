
#include "OSandPlatform.h"

static void printErrorMsg(const char * errMsg);
static void printUsageErrorMsg(uint32_t CFSRValue);
static void printBusFaultErrorMsg(uint32_t CFSRValue);
static void printMemoryManagementErrorMsg(uint32_t CFSRValue);
//static void stackDump(uint32_t stack[]);

void hard_fault_handler(/*uint32_t stack[]*/ void)
{
  static char msg[80];
   redOn(1);
   //if((CoreDebug->DHCSR & 0x01) != 0) {
      printErrorMsg("In Hard Fault Handler\n");
      mysprintf(msg, "SCB_HFSR = 0x%08x\n",(unsigned int)SCB_HFSR);
      printErrorMsg(msg);
      if ((SCB_HFSR & (1 << 30)) != 0) {
         printErrorMsg("Forced Hard Fault\n");
         mysprintf(msg, "SCB_CFSR = 0x%08x\n", (unsigned int)SCB_CFSR );
         printErrorMsg(msg);
         if((SCB_CFSR & 0xFFFF0000) != 0) {
            printUsageErrorMsg(SCB_CFSR);
         } 
         if((SCB_CFSR & 0xFF00) != 0) {
            printBusFaultErrorMsg(SCB_CFSR);
         }
         if((SCB_CFSR & 0xFF) != 0) {
            printMemoryManagementErrorMsg(SCB_CFSR);
         }      
      }  
      //stackDump(stack);
      __asm__ ("BKPT #01");
   //}
      while(1) {
        redOn(0);
        delayms(1000);
        redOn(1);
        delayms(1000);
      }
}

static void printErrorMsg(const char * errMsg)
{
  (void) errMsg;
  DPRINTF(errMsg);
}

static void printUsageErrorMsg(uint32_t CFSRValue)
{
   printErrorMsg("Usage fault: ");
   CFSRValue >>= 16; // right shift to lsb
   
   if((CFSRValue & (1<<9)) != 0) {
      printErrorMsg("Divide by zero\n");
   }
}

static void printBusFaultErrorMsg(uint32_t CFSRValue)
{
   printErrorMsg("Bus fault: ");
   CFSRValue = ((CFSRValue & 0x0000FF00) >> 8); // mask and right shift to lsb
}

void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
   printErrorMsg("Memory Management fault: ");
   CFSRValue &= 0x000000FF; // mask just mem faults
}

/*
void hard_fault_handler(void)
{
   __asm__ ("TST lr, #4");
   __asm__ ("ITE EQ");
   __asm__ ("MRSEQ r0, MSP");
   __asm__ ("MRSNE r0, PSP");
   __asm__ ("B Hard_Fault_Handler");
}
*/
#if 0
enum { r0, r1, r2, r3, r12, lr, pc, psr};

static void stackDump(uint32_t stack[])
{
   static char msg[80];
   mysprintf(msg, "r0  = 0x%08x\n", (unsigned int)stack[r0]);
   printErrorMsg(msg);
   mysprintf(msg, "r1  = 0x%08x\n", (unsigned int)stack[r1]);
   printErrorMsg(msg);
   mysprintf(msg, "r2  = 0x%08x\n", (unsigned int)stack[r2]);
   printErrorMsg(msg);
   mysprintf(msg, "r3  = 0x%08x\n", (unsigned int)stack[r3]);
   printErrorMsg(msg);
   mysprintf(msg, "r12 = 0x%08x\n", (unsigned int)stack[r12]);
   printErrorMsg(msg);
   mysprintf(msg, "lr  = 0x%08x\n", (unsigned int)stack[lr]);
   printErrorMsg(msg);
   mysprintf(msg, "pc  = 0x%08x\n", (unsigned int)stack[pc]);
   printErrorMsg(msg);
   mysprintf(msg, "psr = 0x%08x\n", (unsigned int)stack[psr]);
   printErrorMsg(msg);
}
#endif
