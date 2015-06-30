//*****************************************************************************
//
// startup.c - Boot code for Tek 4330 controller board.
//
//*****************************************************************************
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/scb.h>

#define HARDFAULT_SHOULD_RESET
void selfReset(void);

#if defined (__cplusplus)
// The entry point for the C++ library startup
extern "C" {
  extern void __libc_init_array(void);
}
#endif
#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))
// foo// if CMSIS is being used, then SystemInit() routine// will be called by startup code rather than in application's main()//*****************************************************************************// Forward declaration of the default fault handlers.//*****************************************************************************
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
  void Reset_Handler(void);
  WEAK void NMI_Handler(void);
  WEAK void HardFault_Handler(void *args);
  WEAK void MemManage_Handler(void);
  WEAK void BusFault_Handler(void);
  WEAK void UsageFault_Handler(void);
  WEAK void DebugMon_Handler(void);
  WEAK void IntDefaultHandler(void);

// FreeRTOS vs CMSIS interrupt vector definitions
  extern void xPortPendSVHandler(void);
  extern void PendSV_Handler(void);

  extern void xPortSysTickHandler(void);
  extern void SysTick_Handler(void);

  extern void vPortSVCHandler(void);
  extern void SVC_Handler(void);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. Most are aliased
// to the IntDefaultHandler, which is a 'forever' loop.
//
//*****************************************************************************
  void WWDG_IRQHandler(void) ALIAS(IntDefaultHandler);
  void PVD_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TAMP_STAMP_IRQHandler(void) ALIAS(IntDefaultHandler);
  void RTC_WKUP_IRQHandler(void) ALIAS(IntDefaultHandler);
  void FLASH_IRQHandler(void) ALIAS(IntDefaultHandler);
  void RCC_IRQHandler(void) ALIAS(IntDefaultHandler);
  void EXTI0_IRQHandler(void) ALIAS(IntDefaultHandler);
  void EXTI1_IRQHandler(void) ALIAS(IntDefaultHandler);
  void EXTI2_IRQHandler(void) ALIAS(IntDefaultHandler);
  void EXTI3_IRQHandler(void) ALIAS(IntDefaultHandler);
  void EXTI4_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA1_Stream0_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA1_Stream1_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA1_Stream2_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA1_Stream3_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA1_Stream4_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA1_Stream5_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA1_Stream6_IRQHandler(void) ALIAS(IntDefaultHandler);
  void ADC_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CAN1_TX_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CAN1_RX0_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CAN1_RX1_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CAN1_SCE_IRQHandler(void) ALIAS(IntDefaultHandler);
  void EXTI9_5_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM1_BRK_TIM9_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM1_UP_TIM10_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM1_TRG_COM_TIM11_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM1_CC_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM2_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM3_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM4_IRQHandler(void) ALIAS(IntDefaultHandler);
  void I2C1_EV_IRQHandler(void) ALIAS(IntDefaultHandler);
  void I2C1_ER_IRQHandler(void) ALIAS(IntDefaultHandler);
  void I2C2_EV_IRQHandler(void) ALIAS(IntDefaultHandler);
  void I2C2_ER_IRQHandler(void) ALIAS(IntDefaultHandler);
  void SPI1_IRQHandler(void) ALIAS(IntDefaultHandler);
  void SPI2_IRQHandler(void) ALIAS(IntDefaultHandler);
  void USART1_IRQHandler(void) ALIAS(IntDefaultHandler);
  void USART2_IRQHandler(void) ALIAS(IntDefaultHandler);
  void USART3_IRQHandler(void) ALIAS(IntDefaultHandler);
  void EXTI15_10_IRQHandler(void) ALIAS(IntDefaultHandler);
  void RTC_Alarm_IRQHandler(void) ALIAS(IntDefaultHandler);
  void OTG_FS_WKUP_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM8_BRK_TIM12_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM8_UP_TIM13_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM8_TRG_COM_TIM14_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM8_CC_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA1_Stream7_IRQHandler(void) ALIAS(IntDefaultHandler);
  void FSMC_IRQHandler(void) ALIAS(IntDefaultHandler);
  void SDIO_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM5_IRQHandler(void) ALIAS(IntDefaultHandler);
  void SPI3_IRQHandler(void) ALIAS(IntDefaultHandler);
  void UART4_IRQHandler(void) ALIAS(IntDefaultHandler);
  void UART5_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM6_DAC_IRQHandler(void) ALIAS(IntDefaultHandler);
  void TIM7_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA2_Stream0_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA2_Stream1_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA2_Stream2_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA2_Stream3_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA2_Stream4_IRQHandler(void) ALIAS(IntDefaultHandler);
  void ETH_IRQHandler(void) ALIAS(IntDefaultHandler);
  void ETH_WKUP_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CAN2_TX_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CAN2_RX0_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CAN2_RX1_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CAN2_SCE_IRQHandler(void) ALIAS(IntDefaultHandler);
  void OTG_FS_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA2_Stream5_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA2_Stream6_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DMA2_Stream7_IRQHandler(void) ALIAS(IntDefaultHandler);
  void USART6_IRQHandler(void) ALIAS(IntDefaultHandler);
  void I2C3_EV_IRQHandler(void) ALIAS(IntDefaultHandler);
  void I2C3_ER_IRQHandler(void) ALIAS(IntDefaultHandler);
  void OTG_HS_EP1_OUT_IRQHandler(void) ALIAS(IntDefaultHandler);
  void OTG_HS_EP1_IN_IRQHandler(void) ALIAS(IntDefaultHandler);
  void OTG_HS_WKUP_IRQHandler(void) ALIAS(IntDefaultHandler);
  void OTG_HS_IRQHandler(void) ALIAS(IntDefaultHandler);
  void DCMI_IRQHandler(void) ALIAS(IntDefaultHandler);
  void CRYP_IRQHandler(void) ALIAS(IntDefaultHandler);
  void HASH_RNG_IRQHandler(void) ALIAS(IntDefaultHandler);
  void FPU_IRQHandler(void) ALIAS(IntDefaultHandler);

//*****************************************************************************
// External declaration for the pointer to the stack top from the Linker Script
//*****************************************************************************
  extern void __StackTop(void);

//*****************************************************************************
#if defined (__cplusplus)
}				// extern "C"
#endif
//*****************************************************************************// The vector table. This one's in Flash. placed at offset 0x0 so it's // available right at bootup. We'll copy it to RAM and change VTOR to point// to the RAM copy in the reset handler. Then runtime code can change// interrupt vectors. //*****************************************************************************
    extern void (*const FlashVectors[]) (void);

//static FILE real_stdout;

__attribute__ ((section(".isr_vector")))
void (*const FlashVectors[]) (void) =
{
  // Core Level - CM4/CM3
  &__StackTop,			// The initial stack pointer
      Reset_Handler,		// The reset handler
      NMI_Handler,		// The NMI handler
      (void (*)(void)) HardFault_Handler,	// The hard fault handler
      MemManage_Handler,	// The MPU fault handler
      BusFault_Handler,		// The bus fault handler
      UsageFault_Handler,	// The usage fault handler
      0,			// Reserved
      0,			// Reserved
      0,			// Reserved
      0,			// Reserved
      SVC_Handler,		// SVCall handler
      DebugMon_Handler,		// Debug monitor handler
      0,			// Reserved
      PendSV_Handler,		// The PendSV handler
      SysTick_Handler,		// The SysTick handler
      WWDG_IRQHandler,		// Chip Level - STM32
      PVD_IRQHandler,		// 16 Window Watchdog
      TAMP_STAMP_IRQHandler,	// 17 PVD through EXTI Line detection 
      RTC_WKUP_IRQHandler,	// 18 Tamper and TimeStamps through the EXTI line 
      FLASH_IRQHandler,		// 19 RTC Wakeup through the EXTI line 
      RCC_IRQHandler,		// 20 FLASH                        
      EXTI0_IRQHandler,		// 21 RCC                          
      EXTI1_IRQHandler,		// 22 EXTI Line0                   
      EXTI2_IRQHandler,		// 23 EXTI Line1                   
      EXTI3_IRQHandler,		// 24 EXTI Line2                   
      EXTI4_IRQHandler,		// 25 EXTI Line3                   
      DMA1_Stream0_IRQHandler,	// 26 EXTI Line4                   
      DMA1_Stream1_IRQHandler,	// 27 DMA1 Stream 0                
      DMA1_Stream2_IRQHandler,	// 28 DMA1 Stream 1                
      DMA1_Stream3_IRQHandler,	// 29 DMA1 Stream 2                
      DMA1_Stream4_IRQHandler,	// 30 DMA1 Stream 3                
      DMA1_Stream5_IRQHandler,	// 31 DMA1 Stream 4                
      DMA1_Stream6_IRQHandler,	// 32 DMA1 Stream 5                
      ADC_IRQHandler,		// 33 DMA1 Stream 6                
      CAN1_TX_IRQHandler,	// 34 ADC1, ADC2 and ADC3s         
      CAN1_RX0_IRQHandler,	// 35 CAN1 TX                      
      CAN1_RX1_IRQHandler,	// 36 CAN1 RX0                     
      CAN1_SCE_IRQHandler,	// 37 CAN1 RX1                     
      EXTI9_5_IRQHandler,	// 38 CAN1 SCE                     
      TIM1_BRK_TIM9_IRQHandler,	// 39 External Line[9:5]s          
      TIM1_UP_TIM10_IRQHandler,	// 40 TIM1 Break and TIM9          
      TIM1_TRG_COM_TIM11_IRQHandler,	// 41 TIM1 Update and TIM10        
      TIM1_CC_IRQHandler,	// 42 TIM1 Trigger and Commutation and TIM11 
      TIM2_IRQHandler,		// 43 TIM1 Capture Compare         
      TIM3_IRQHandler,		// 44 TIM2                         
      TIM4_IRQHandler,		// 45 TIM3                         
      I2C1_EV_IRQHandler,	// 46 TIM4                         
      I2C1_ER_IRQHandler,	// 47 I2C1 Event                   
      I2C2_EV_IRQHandler,	// 48 I2C1 Error                   
      I2C2_ER_IRQHandler,	// 49 I2C2 Event                   
      SPI1_IRQHandler,		// 50 I2C2 Error                   
      SPI2_IRQHandler,		// 51 SPI1                         
      USART1_IRQHandler,	// 52 SPI2                         
      USART2_IRQHandler,	// 53 USART1                       
      USART3_IRQHandler,	// 54 USART2                       
      EXTI15_10_IRQHandler,	// 55 USART3                       
      RTC_Alarm_IRQHandler,	// 56 External Line[15:10]s        
      OTG_FS_WKUP_IRQHandler,	// 57 RTC Alarm (A and B) through EXTI Line 
      TIM8_BRK_TIM12_IRQHandler,	// 58 USB OTG FS Wakeup through EXTI line 
      TIM8_UP_TIM13_IRQHandler,	// 59 TIM8 Break and TIM12         
      TIM8_TRG_COM_TIM14_IRQHandler,	// 60 TIM8 Update and TIM13        
      TIM8_CC_IRQHandler,	// 61 TIM8 Trigger and Commutation and TIM14 
      DMA1_Stream7_IRQHandler,	// 62 TIM8 Capture Compare         
      FSMC_IRQHandler,		// 63 DMA1 Stream7                 
      SDIO_IRQHandler,		// 64 FSMC                         
      TIM5_IRQHandler,		// 65 SDIO                         
      SPI3_IRQHandler,		// 66 TIM5                         
      UART4_IRQHandler,		// 67 SPI3                         
      UART5_IRQHandler,		// 68 UART4                        
      TIM6_DAC_IRQHandler,	// 69 UART5                        
      TIM7_IRQHandler,		// 70 TIM6 and DAC1&2 underrun errors 
      DMA2_Stream0_IRQHandler,	// 71 TIM7                         
      DMA2_Stream1_IRQHandler,	// 72 DMA2 Stream 0                
      DMA2_Stream2_IRQHandler,	// 73 DMA2 Stream 1                
      DMA2_Stream3_IRQHandler,	// 74 DMA2 Stream 2                
      DMA2_Stream4_IRQHandler,	// 75 DMA2 Stream 3                
      ETH_IRQHandler,		// 76 DMA2 Stream 4                
      ETH_WKUP_IRQHandler,	// 77 Ethernet                     
      CAN2_TX_IRQHandler,	// 78 Ethernet Wakeup through EXTI line 
      CAN2_RX0_IRQHandler,	// 79 CAN2 TX                      
      CAN2_RX1_IRQHandler,	// 80 CAN2 RX0                     
      CAN2_SCE_IRQHandler,	// 81 CAN2 RX1                     
      OTG_FS_IRQHandler,	// 82 CAN2 SCE                     
      DMA2_Stream5_IRQHandler,	// 83 USB OTG FS                   
      DMA2_Stream6_IRQHandler,	// 84 DMA2 Stream 5                
      DMA2_Stream7_IRQHandler,	// 85 DMA2 Stream 6                
      USART6_IRQHandler,	// 86 DMA2 Stream 7                
      I2C3_EV_IRQHandler,	// 87 USART6                       
      I2C3_ER_IRQHandler,	// 88 I2C3 event                   
      OTG_HS_EP1_OUT_IRQHandler,	// 89 I2C3 error                   
      OTG_HS_EP1_IN_IRQHandler,	// 90 USB OTG HS End Point 1 Out   
      OTG_HS_WKUP_IRQHandler,	// 91 USB OTG HS End Point 1 In    
      OTG_HS_IRQHandler,	// 92 USB OTG HS Wakeup through EXTI 
      DCMI_IRQHandler,		// 93 USB OTG HS                   
      CRYP_IRQHandler,		// 94 DCMI                         
      HASH_RNG_IRQHandler,	// 95 CRYP crypto                  
      FPU_IRQHandler		// 96 Hash and Rng                 
      // 97 FPU                          
};

//void (*RAMVectors[NVECS])(void) __attribute__ (( aligned(32) ));
#define NVECS (sizeof(FlashVectors)/sizeof(FlashVectors[0]))
//68
void (*const RAMVectors[NVECS]) (void) __attribute__ ((section("vtable")));

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
void Reset_Handler(void)
{

  //  Call SystemInit() for clocking/memory setup prior to scatter load 
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

    unsigned int *Dest = (unsigned int *) ExeAddr;
    unsigned int *Src = (unsigned int *) LoadAddr;
    for (loop = 0; loop < SectionLen; loop += sizeof(unsigned int)) {
      *Dest++ = *Src++;
    }
  }
  // At this point, SectionTableAddr = &__bss_section_table;
  // Zero fill the bss segments
  while (SectionTableAddr < &__bss_section_table_end) {
    ExeAddr = *SectionTableAddr++;
    SectionLen = *SectionTableAddr++;

    unsigned int *Dest = (unsigned int *) ExeAddr;
    for (loop = 0; loop < SectionLen; loop += sizeof(unsigned int)) {
      *Dest++ = 0;
    }
  }

  // copy the interrupt vector table to RAM and point to it
  //memcpy(&RAMVectors,&FlashVectors,NVECS*sizeof( void(*)(void) ) );
  unsigned int *Dest = (unsigned int *) &RAMVectors;
  unsigned int *Src = (unsigned int *) &FlashVectors;
  unsigned int length = NVECS * sizeof(void (*)(void));
  for (loop = 0; loop < length; loop += sizeof(unsigned int)) {
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
  //currentClockRate = Chip_Clock_GetRate(CLK_MX_MXCORE);	// should be 204 MHz, or close thereto
  //ClockRateHigh = 1;
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
  while (1) {
  }
}

typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;			//  Link Register. 
  uint32_t pc;			//  Program Counter. 
  uint32_t psr;			//  Program Status Register. 
} hard_fault_stack_t;

__attribute__ ((naked))
void hard_fault_handler(void)
{
  __asm__("TST LR, #4");
  __asm__("ITE EQ");
  __asm__("MRSEQ R0, MSP");
  __asm__("MRSNE R0, PSP");
  __asm__("B hard_fault_handler_c");
}

volatile hard_fault_stack_t *hard_fault_stack_pt;

__attribute__ ((section(".after_vectors")))
void HardFault_Handler(void *args)
{
  volatile int j = 0;
  volatile int count = 30;
  //  hard_fault_stack_pt contains registers saved before the hard fault 
  hard_fault_stack_pt = (hard_fault_stack_t *) args;
  // see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/Cihcfefj.html
  // see: http://www.keil.com/appnotes/files/apnt209.pdf
  // args[0-7]: r0, r1, r2, r3, r12, lr, pc, psr
  // Other interesting registers to examine:
  //    CFSR: Configurable Fault Status Register (0xE000ED28)
  //    HFSR: Hard Fault Status Register (0xE000ED2C)
  //    DFSR: Debug Fault Status Register 
  //    AFSR: Auxiliary Fault Status Register (0xE000ED3C)
  //    MMAR: MemManage Fault Address Register (0xE000ED34)
  //    BFAR: Bus Fault Address Register (0xE000ED38)

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
    //AttnLED (0, 0, 255);
    for (j = 1000000; j > 0; j--);
    //AttnLED (0, 0, 0);
    for (j = 1000000; j > 0; j--);
  }
  selfReset();
}

__attribute__ ((section(".after_vectors")))
void MemManage_Handler(void)
{
  while (1) {
  }
}

__attribute__ ((section(".after_vectors")))
void BusFault_Handler(void)
{
  while (1) {
  }
}

__attribute__ ((section(".after_vectors")))
void UsageFault_Handler(void)
{
  while (1) {
  }
}

__attribute__ ((section(".after_vectors")))
void DebugMon_Handler(void)
{
  while (1) {
  }
}


//*****************************************************************************
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
  while (1) {
  }
}

void selfReset(void)
{
  //Chip_RGU_TriggerReset (RGU_MASTER_RST);
}



 //****************************************************************************
 // S y s t e m I n i t
 //****************************************************************************

 /**
 * SystemInit() is called prior to the application and sets up system
 * clocking, memory, and any resources needed prior to the application
 * starting.
 */
void SystemInit(void)
{
#if defined(CORE_M3) || defined(CORE_M4)
#if defined(__FPU_PRESENT) && __FPU_PRESENT == 1
  fpuInit();
#endif
#endif
  
  // Now setup the clocks ...
  // Discovery is 8MHz crystal, use 120MHz core
  rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_OTGFS);

  return;
}
