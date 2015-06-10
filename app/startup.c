//*****************************************************************************
//
// startup.c - Boot code for Tek 4330 controller board.
//
//*****************************************************************************
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "tek_board_init.h"

#define HARDFAULT_SHOULD_RESET
void selfReset(void);
extern void requestHighClock(void); //main.c

#if defined (__cplusplus)
// The entry point for the C++ library startup
extern "C" {
	extern void __libc_init_array(void);
}
#endif

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

// foo
// if CMSIS is being used, then SystemInit() routine
// will be called by startup code rather than in application's main()
//*****************************************************************************
// Forward declaration of the default fault handlers.
//*****************************************************************************

extern void SystemInit(void);

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
// main() is the entry point for Newlib based applications
//*****************************************************************************
extern int main(void);

//*****************************************************************************
// Forward declaration of the default handlers. These are aliased using the
// linker "weak" attribute:  if the application defines a handler with the 
// same name, it will take precedence over the weak definition
//*****************************************************************************
     void 	Reset_Handler(void);
WEAK void 	NMI_Handler(void);
WEAK void 	HardFault_Handler(void *args);
WEAK void 	MemManage_Handler(void);
WEAK void 	BusFault_Handler(void);
WEAK void 	UsageFault_Handler(void);
WEAK void   DebugMon_Handler(void);
WEAK void   IntDefaultHandler(void);

// FreeRTOS vs CMSIS interrupt vector definitions
extern void xPortPendSVHandler( void );
extern void PendSV_Handler(void);

extern void xPortSysTickHandler( void );
extern void SysTick_Handler(void);

extern void vPortSVCHandler( void );
extern void SVC_Handler(void);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. Most are aliased
// to the IntDefaultHandler, which is a 'forever' loop.
//
//*****************************************************************************
void DAC_IRQHandler(void)     	  ALIAS(IntDefaultHandler);
void MX_CORE_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void DMA_IRQHandler(void)     	  ALIAS(IntDefaultHandler);
void FLASHEEPROM_IRQHandler(void) ALIAS(IntDefaultHandler);
void ETH_IRQHandler(void)  		  ALIAS(IntDefaultHandler);
void SDIO_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void LCD_IRQHandler(void)  		  ALIAS(IntDefaultHandler);
void USB0_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void USB1_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void SCT_IRQHandler(void)  		  ALIAS(IntDefaultHandler);
void RIT_IRQHandler(void)  		  ALIAS(IntDefaultHandler);
void TIMER0_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void TIMER1_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void TIMER2_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void TIMER3_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void MCPWM_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void ADC0_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void I2C0_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void I2C1_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void SPI_IRQHandler (void) 		  ALIAS(IntDefaultHandler);
void ADC1_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void SSP0_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void SSP1_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void UART0_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void UART1_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void UART2_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void UART3_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void I2S0_IRQHandler(void)  	  ALIAS(IntDefaultHandler);
void I2S1_IRQHandler(void)  	  ALIAS(IntDefaultHandler);
void SPIFI_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void SGPIO_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO0_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO1_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO2_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO3_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO4_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO5_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO6_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO7_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GINT0_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GINT1_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void EVRT_IRQHandler(void) 	 	  ALIAS(IntDefaultHandler);
void CAN1_IRQHandler(void) 	 	  ALIAS(IntDefaultHandler);
void ATIMER_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void RTC_IRQHandler(void) 	 	  ALIAS(IntDefaultHandler);
void WDT_IRQHandler(void) 	 	  ALIAS(IntDefaultHandler);
void CAN0_IRQHandler(void)   	  ALIAS(IntDefaultHandler);
void QEI_IRQHandler(void)    	  ALIAS(IntDefaultHandler);

//*****************************************************************************
// External declaration for the pointer to the stack top from the Linker Script
//*****************************************************************************
extern void __StackTop(void);

//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif

//*****************************************************************************
// The vector table. This one's in Flash. placed at offset 0x0 so it's 
// available right at bootup. We'll copy it to RAM and change VTOR to point
// to the RAM copy in the reset handler. Then runtime code can change
// interrupt vectors. 
//*****************************************************************************
extern void (* const FlashVectors[])(void);

//static FILE real_stdout;

__attribute__ ((section(".isr_vector")))
void (* const FlashVectors[])(void) = {
	// Core Level - CM4/CM3
	&__StackTop,	                           // The initial stack pointer
	Reset_Handler,						// The reset handler
	NMI_Handler,					// The NMI handler
	(void(*)(void))HardFault_Handler,				// The hard fault handler
	MemManage_Handler,				// The MPU fault handler
	BusFault_Handler,				// The bus fault handler
	UsageFault_Handler,				// The usage fault handler
	0,								// Reserved
	0,								// Reserved
	0,								// Reserved
	0,								// Reserved
	SVC_Handler,				// SVCall handler
	DebugMon_Handler,				// Debug monitor handler
	0,								// Reserved
	PendSV_Handler,				// The PendSV handler
	SysTick_Handler,			// The SysTick handler
	// Chip Level - LPC18xx/43xx
	DAC_IRQHandler,					// 16 D/A Converter
	MX_CORE_IRQHandler,				// 17 CortexM4/M0 (LPC43XX ONLY)
	DMA_IRQHandler,					// 18 General Purpose DMA
	0,								// 19 Reserved
	FLASHEEPROM_IRQHandler,			// 20 ORed flash Bank A, flash Bank B, EEPROM interrupts
	ETH_IRQHandler,					// 21 Ethernet
	SDIO_IRQHandler,				// 22 SD/MMC
	LCD_IRQHandler,					// 23 LCD
	USB0_IRQHandler,				// 24 USB0
	USB1_IRQHandler,				// 25 USB1
	SCT_IRQHandler,					// 26 State Configurable Timer
	RIT_IRQHandler,					// 27 Repetitive Interrupt Timer
	TIMER0_IRQHandler,				// 28 Timer0
	TIMER1_IRQHandler,				// 29 Timer 1
	TIMER2_IRQHandler,				// 30 Timer 2
	TIMER3_IRQHandler,				// 31 Timer 3
	MCPWM_IRQHandler,				// 32 Motor Control PWM
	ADC0_IRQHandler,				// 33 A/D Converter 0
	I2C0_IRQHandler,				// 34 I2C0
	I2C1_IRQHandler,				// 35 I2C1
	SPI_IRQHandler,					// 36 SPI (LPC43XX ONLY)
	ADC1_IRQHandler,				// 37 A/D Converter 1
	SSP0_IRQHandler,				// 38 SSP0
	SSP1_IRQHandler,				// 39 SSP1
	UART0_IRQHandler,				// 40 UART0
	UART1_IRQHandler,				// 41 UART1
	UART2_IRQHandler,				// 42 UART2
	UART3_IRQHandler,				// 43 USRT3
	I2S0_IRQHandler,				// 44 I2S0
	I2S1_IRQHandler,				// 45 I2S1
	SPIFI_IRQHandler,				// 46 SPI Flash Interface
	SGPIO_IRQHandler,				// 47 SGPIO (LPC43XX ONLY)
	GPIO0_IRQHandler,				// 48 GPIO0
	GPIO1_IRQHandler,				// 49 GPIO1
	GPIO2_IRQHandler,				// 50 GPIO2
	GPIO3_IRQHandler,				// 51 GPIO3
	GPIO4_IRQHandler,				// 52 GPIO4
	GPIO5_IRQHandler,				// 53 GPIO5
	GPIO6_IRQHandler,				// 54 GPIO6
	GPIO7_IRQHandler,				// 55 GPIO7
	GINT0_IRQHandler,				// 56 GINT0
	GINT1_IRQHandler,				// 57 GINT1
	EVRT_IRQHandler,				// 58 Event Router
	CAN1_IRQHandler,				// 59 C_CAN1
	0,								// 60 Reserved
	0,				                // 61 Reserved
	ATIMER_IRQHandler,				// 62 ATIMER
	RTC_IRQHandler,					// 63 RTC
	0,								// 64 Reserved
	WDT_IRQHandler,					// 65 WDT
	0,								// 66 Reserved
	CAN0_IRQHandler,				// 67 C_CAN0
	QEI_IRQHandler,					// 68 QEI
};

//void (*RAMVectors[NVECS])(void) __attribute__ (( aligned(32) ));
#define NVECS (sizeof(FlashVectors)/sizeof(FlashVectors[0]))
//68
void (* const RAMVectors[NVECS])(void) __attribute__ (( section ("vtable") ));

extern void SystemInit(void);
extern void __data_start__;
extern void __data_end__;
extern uint32_t currentClockRate;
extern int32_t ClockRateHigh;

//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;
//*****************************************************************************
// Reset entry point: sets up the crt and/or the c/c++ libraries
//*****************************************************************************
void Reset_Handler(void) {
    
    /* Call SystemInit() for clocking/memory setup prior to scatter load */
    SystemInit();

    //
    // Copy the data sections from flash to SRAM.
    //
    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *SectionTableAddr;
    unsigned int loop;


    // Load base address of Global Section Table
    SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
    while (SectionTableAddr < &__data_section_table_end) {
      LoadAddr = *SectionTableAddr++;
      ExeAddr = *SectionTableAddr++;
      SectionLen = *SectionTableAddr++;
 
     unsigned int *Dest = (unsigned int *)ExeAddr;
     unsigned int *Src = (unsigned int *)LoadAddr;
     for ( loop=0; loop<SectionLen; loop += sizeof(unsigned int) ) {
        *Dest++ = *Src++;
      }
    }
    // At this point, SectionTableAddr = &__bss_section_table;
    // Zero fill the bss segments
    while (SectionTableAddr < &__bss_section_table_end) {
      ExeAddr = *SectionTableAddr++;
      SectionLen = *SectionTableAddr++;

      unsigned int *Dest = (unsigned int *)ExeAddr;
      for ( loop=0; loop<SectionLen; loop += sizeof(unsigned int) ) {
        *Dest++ = 0;
      }
    }

    // copy the interrupt vector table to RAM and point to it
    //memcpy(&RAMVectors,&FlashVectors,NVECS*sizeof( void(*)(void) ) );
    unsigned int *Dest = (unsigned int *)&RAMVectors;
    unsigned int *Src = (unsigned int *)&FlashVectors;
    unsigned int length = NVECS*sizeof( void(*)(void) );
    for ( loop=0; loop<length; loop += sizeof(unsigned int))  {
      *Dest++ = *Src++;
    }
#if defined(CORE_M3) || defined(CORE_M4)
    unsigned int *pSCB_VTOR = (unsigned int *) 0xE000ED08;
    *pSCB_VTOR = (unsigned int) &RAMVectors;
#endif

#if defined (__cplusplus)
	//
	// Call C++ library initialisation
	//
	__libc_init_array();
#endif

	// re-assert static values essential to main()
	currentClockRate=Chip_Clock_GetRate(CLK_MX_MXCORE);  // should be 204 MHz, or close thereto
	ClockRateHigh=1;
	// Call main()
	main();

	//
	// main() shouldn't return, but if it does, we'll just enter an infinite loop 
	//
	while (1) {
		;
	}
}

//*****************************************************************************
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void NMI_Handler(void)
{
    while(1)
    {
    }
}

typedef struct
{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr; /* Link Register. */
	uint32_t pc; /* Program Counter. */
	uint32_t psr;/* Program Status Register. */
} hard_fault_stack_t;

__attribute__((naked))
void hard_fault_handler(void) {
	__asm__("TST LR, #4");
	__asm__("ITE EQ");
	__asm__("MRSEQ R0, MSP");
	__asm__("MRSNE R0, PSP");
	__asm__("B hard_fault_handler_c");
}

volatile hard_fault_stack_t* hard_fault_stack_pt;

extern void w25q64_setup(void);
__attribute__ ((section(".after_vectors")))
void HardFault_Handler(void *args)
{
  volatile int j=0;
  volatile int count = 30;
  /* hard_fault_stack_pt contains registers saved before the hard fault */
  hard_fault_stack_pt = (hard_fault_stack_t*)args;
  // see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/Cihcfefj.html
  // see: http://www.keil.com/appnotes/files/apnt209.pdf
  // args[0-7]: r0, r1, r2, r3, r12, lr, pc, psr
  // Other interesting registers to examine:
  //	CFSR: Configurable Fault Status Register (0xE000ED28)
  //	HFSR: Hard Fault Status Register (0xE000ED2C)
  //	DFSR: Debug Fault Status Register 
  //	AFSR: Auxiliary Fault Status Register (0xE000ED3C)
  //	MMAR: MemManage Fault Address Register (0xE000ED34)
  //	BFAR: Bus Fault Address Register (0xE000ED38)
  
  /*
    if( SCB->HFSR & SCB_HFSR_FORCED ) {	
    if( SCB->CFSR & SCB_CFSR_BFSR_BFARVALID ) {
    SCB->BFAR;
    if( SCB->CFSR & CSCB_CFSR_BFSR_PRECISERR ) {
    }
    }
    }
  */
#ifdef HARDFAULT_SHOULD_RESET
  while (count--) {
#else 
  while (count) {
#endif
    AttnLED(0,0,255);
    for (j=1000000; j>0; j--) ;
    AttnLED(0,0,0);
    for (j=1000000; j>0; j--) ;
  }  
  selfReset();
}
__attribute__ ((section(".after_vectors")))
void MemManage_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void BusFault_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void UsageFault_Handler(void)
{
    while(1)
    {
    }
}

__attribute__ ((section(".after_vectors")))
void DebugMon_Handler(void)
{
    while(1)
    {
    }
}


//*****************************************************************************
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
    while(1)
    {
    }
}

void selfReset(void)
{
  Chip_RGU_TriggerReset(RGU_MASTER_RST);
}
