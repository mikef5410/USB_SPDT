/*******************************************************************************
*           Copyright (C) 2015 Michael R. Ferrara, All rights reserved.
*
*                       Santa Rosa, CA 95404
*                       Tel:(707)536-1330
*
* Filename:     eeprom9366.c
*
* Description: EEPROM9366 "driver"
*
*******************************************************************************/
//#define TRACE_PRINT 1

#include "OSandPlatform.h"

#define GLOBAL_EEPROM9366
#include "eeprom9366.h"


static uint32_t writeEnabled = 0;

static inline void assertCS()
{
  taskENTER_CRITICAL();
  gpio_set(GPIOB,GPIO9);
  return;
}

static inline void deassertCS()
{
  gpio_clear(GPIOB,GPIO9);
  taskEXIT_CRITICAL();
  return;
}

static inline uint32_t spiTransaction(uint8_t command, uint8_t address, uint8_t data)
{
  uint8_t rbyte = 0;

  assertCS();
  // Byte 1 ... 3 bits of command
  spi_send(SPI2,command);

  // Byte 2 ... 8 bits of address,
  spi_send(SPI2, address);

  //Byte 3 ... data
  rbyte = spi_xfer(SPI2, data);

  deassertCS();
  
  return(rbyte);
}

static inline void spiCMD(uint8_t command, uint8_t address)
{

  assertCS();
  
  // Byte 1 ... 3 bits of command
  spi_send(SPI2,command);

  // Byte 2 ... 8 bits of address
  spi_send(SPI2, address);

  deassertCS();
  return;
}

static inline void writeEnable()
{
  if (writeEnabled) return;
  spiCMD(0x4,0xC0);
  writeEnabled=1;
  return;
}

EEPROM9366GLOBAL void eeprom9366_eraseAll()
{
  writeEnable();
  spiCMD(0x4, 0x80);
  delayms(6);
}

EEPROM9366GLOBAL void eeprom9366_erase(uint8_t address)
{
  writeEnable();
  spiCMD(0x7,address);
  delayms(6);
  return;
}


EEPROM9366GLOBAL void eeprom9366_write(uint8_t address, uint8_t data)
{
  writeEnable();
  (void)spiTransaction(0x5,address,data);
  delayms(6);
  return;
}


EEPROM9366GLOBAL uint8_t eeprom9366_read(uint8_t address)
{
  uint8_t res = spiTransaction(0x6,address,0);
  return(res);
}

EEPROM9366GLOBAL void eeprom9366_init()
{

  //SPI2 pins for EEPROM (Alternate function 5)
  //PB9 is SPI2NSS, PB10 is SPI2SCK
  //PB14 is SPI2MISO, PB15 SPI2MOSI
  //PCLK is 48MHz, so we need to select a baud rate divider
  //such that SPI CLK is less than 2MHz (32 -> 1.5MHz)
  gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  GPIO10 | GPIO14 | GPIO15);
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9); //Software Slave-Select
  gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO9);
  gpio_clear(GPIOB,GPIO9);
  
  gpio_set_af(GPIOB, GPIO_AF5, GPIO10 | GPIO14 | GPIO15);
  gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
                                 GPIO10 | GPIO15);
  rcc_periph_clock_enable(RCC_SPI2);

  spi_reset(SPI2);
  spi_set_unidirectional_mode(SPI2);
  spi_disable_crc(SPI2);
  spi_set_next_tx_from_buffer(SPI2);
  spi_set_full_duplex_mode(SPI2);
  spi_send_msb_first(SPI2);
  spi_set_baudrate_prescaler(SPI2,SPI_CR1_BAUDRATE_FPCLK_DIV_32);
  spi_set_master_mode(SPI2);
  spi_disable_tx_buffer_empty_interrupt(SPI2);
  spi_disable_rx_buffer_not_empty_interrupt(SPI2);
  spi_disable_error_interrupt(SPI2);
  spi_set_standard_mode(SPI2,0);

  spi_enable(SPI2);
  return;
}

#ifdef TESTEEPROM
EEPROM9366GLOBAL void eeprom9366_test()
{
  uint8_t d;
  
  myprintf("EEProm Test\n");
  eeprom9366_eraseAll();
  myprintf("Erase complete\n");
  d = eeprom9366_read(0x10);
  myprintf("EEPROM read after erase: 0x%x\n",d);
  eeprom9366_write(0x10,0xde);
  d = eeprom9366_read(0x10);
  myprintf("EEPROM read after write (0xDE): 0x%x\n",d);
  eeprom9366_erase(0x10);
 
  d = eeprom9366_read(0x10);
  myprintf("EEPROM read after cell erase: 0x%x\n",d);

  return;
}
#endif
