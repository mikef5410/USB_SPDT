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
  gpio_set(GPIOB,GPIO9);
  return;
}

static inline void deassertCS()
{
  gpio_clear(GPIOB,GPIO9);
  return;
}

static inline uint32_t spiTransaction(uint8_t command, uint8_t address, uint8_t data)
{
  uint8_t obyte;
  uint8_t ibyte;
  uint8_t rbyte = 0;

  assertCS();
  // Byte 1 ... 3 bits of command, 5 bits of address
  obyte = ((command << 5)&0xe0) | ((address >> 3)&0x1f);
  spi_send(SPI2,obyte);
  (void) spi_read(SPI2);

  // Byte 2 ... last 3 bits of address, 5 bits of data
  obyte = (address<<5)&0xe0;
  obyte |= ((data >> 3) & 0x1f);
  spi_send(SPI2, obyte);
  ibyte = spi_read(SPI2);
  rbyte = (ibyte << 3) & 0xf8;

  //Byte 3 ... last 3 bits of data
  obyte = (data << 5 ) & 0xe0;
  spi_send(SPI2, obyte);
  ibyte = spi_read(SPI2);
  rbyte |= ((ibyte >> 5) & 0x7);

  deassertCS();
  
  return(rbyte);
}

static inline void spiCMD(uint8_t command, uint8_t address)
{
  uint8_t obyte;

  assertCS();
  
  // Byte 1 ... 3 bits of command, 5 bits of address
  obyte = ((command << 5)&0xe0) | ((address >> 3)&0x1f);
  spi_send(SPI2,obyte);
  (void) spi_read(SPI2);

  // Byte 2 ... last 3 bits of address, 5 bits of (zero) data
  obyte = (address<<5)&0xe0;
  spi_send(SPI2, obyte);
  (void) spi_read(SPI2);

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
  uint32_t cr_tmp = 0;
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

  SPI_CR1(SPI2) = 0;
  cr_tmp = SPI_CR1_BAUDRATE_FPCLK_DIV_32 | SPI_CR1_MSTR | SPI_CR1_CPHA 
     | SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE ;

  SPI_CR1(SPI2) = cr_tmp;
  cr_tmp |= SPI_CR1_SPE ;
  SPI_CR1(SPI2) = cr_tmp;

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
