/**
 * 
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

// Standard includes
// #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter" 

#if defined (__USE_CMSIS)
	#include "system_LPC18xx_43xx.h"
#endif
// #include "lpc43xx_uart.h"
// #include "lpc43xx_gpio.h"
// #include "lpc43xx_nvic.h"
// #include "lpc43xx_ssp.h"
// #include "lpc43xx_adc.h"
// #include "lpc43xx_wwdt.h"
// #include "lpc43xx_emac.h"
// #include "lpc43xx_pinsel.h"

// FreeRTOS includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-sign"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#pragma GCC diagnostic pop

// USB-related includes
// Mike's chip.h turns off warnings, and includes chip_lpc43xx.h, which includes all peripheral library header files
#include "chip.h"
#include "cmsis.h"       // jimj: make sure this comes from: external/lpcopen/lpcopen/software/lpc_core/lpc_chip/chip_18xx_43xx/
#include "core_cm4.h"    // jimj: make sure this comes from: external/lpcopen/lpcopen/software/CMSIS/CMSIS/Include/

#include "board_api.h"

#pragma GCC diagnostic pop

// #define STACK_SIZE  0x400
// #define HEAP_SIZE   0x4000

// Define FreeRTOS task and interrupt priorities: 
//    task priorities are used by FreeRTOS_config.h, 
//    and could probably be moved there
// jimj
//
// In FreeRTOS, the higher the number, the higher the priority
//                                                          // an LCD is not used, but this is a good example of why the priority is
#define TASK_PRIO_LWIP_THREAD     (tskIDLE_PRIORITY + 5UL)  // IP protocol layer => highest (not used)
#define TASK_PRIO_USBDEVICE       (tskIDLE_PRIORITY + 4UL)  // USB next hightest, etc
#define TASK_PRIO_IPC_DISPATCH    (tskIDLE_PRIORITY + 3UL)  // used for M4-to-M0 sync (IPC)
#define TASK_PRIO_ETHERNET        (tskIDLE_PRIORITY + 2UL)  // network phy (not used)
#define TASK_PRIO_BLINKY_EVENT    (tskIDLE_PRIORITY + 1UL)
#define TASK_PRIO_KBD             (tskIDLE_PRIORITY + 0UL)

// Priority of IRQs used
//   the lower the number, the higher the priority
//   IPC and ETHERNET are not used, but are included here for didactic value
#define IRQ_PRIO_IPC              7   // not used (no M0 code at this point)
#define IRQ_PRIO_ETHERNET         6
#define IRQ_PRIO_USBDEV           5

// FAT Filesystem needs RAM DISK: size & location 
#define RAMDISK_LOCATION    ".bss.RAM4"
#define RAMDISK_SIZE        0x2000

/* Base address of various flashes */
#define SPIFI_BASE_ADDR      0x14000000
#define XFLASH_BASE_ADDR     0x1C000000
#define IFLASH_BASE_ADDR     0x1A000000

#define UART0_BAUDRATE    57600   //  default baudtrate for UART0
#define UART3_BAUDRATE    115200  //  default baudtrate for UART3
#define USB_VCOM_BAUDRATE 115200  //  default baudtrate for USB virtual serial port
#define LOCAL_TIMEZONE UTC_P_0300 //  timezone used when you syncronize the time @see enum TimeZoneNumber


// For dual-core, the image on a 4330 is placed in SPIFI rom/flash
// start of equivalent macros from: #include "dualcore_common.h"
#if (!defined(TARGET_SPIFI) && !defined(TARGET_IFLASH) && !defined(TARGET_XFLASH))
    #define TARGET_SPIFI
	#define IMAGE_BASE_ADDR (SPIFI_BASE_ADDR)
#endif

#define LOCATE_ATX(x)     __attribute__((section(x)))
#define LOCATE_AT(x) LOCATE_ATX(x)
#define WEAK_SYMBOL      __attribute__((weak))


#ifdef __cplusplus
extern "C" {
#endif

void MSleep(int32_t msecs);
#ifdef __cplusplus
}
#endif
// end include  from dual_core_common.h


/**
 * Thread-safe printf
 */
#define TSPRINTF(...) taskENTER_CRITICAL(); \
					printf(__VA_ARGS__); \
					taskEXIT_CRITICAL()


#endif /* APP_CONFIG_H_ */
