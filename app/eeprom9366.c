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

// This device has 9 bits of address, led by a three bit cmd.
// Data is 8 bit bytes.


static uint32_t writeEnabled = 0;

static inline void assertCS()
{
  //taskENTER_CRITICAL();
  gpio_set(GPIOB,GPIO9);
  //delayms(1); //Tcss
  return;
}

static inline void deassertCS()
{
  delayms(1); //Tcsl
  gpio_clear(GPIOB,GPIO9);
  //taskEXIT_CRITICAL();
  return;
}

static inline uint8_t spiTransaction(uint8_t command, uint16_t address, uint8_t data)
{
  uint8_t rbyte = 0;

  uint8_t b1 = (command<<1) | ((address & 0x100)>>8);
  uint8_t b2 = address & 0xff;
  
  printf("OUT: 0x%02x, 0x%04x, 0x%02x\n",command, address, data);
  printf("Trans: 0x%02x 0x%02x 0x%02x\n",b1,b2,data);
  assertCS();
  // Byte 1 ... 3 bits of command + 9th bit of address
  (void)spi_xfer(SPI2, b1 );

  // Byte 2 ... 8 bits of address,
  (void)spi_xfer(SPI2, b2);

  // Wait for Tx empty
  //while (!(SPI_SR(SPI2) & SPI_SR_TXE));

  rbyte = spi_xfer(SPI2, data);
  
  while ((SPI_SR(SPI2) & SPI_SR_BSY));

  deassertCS();
  printf("IN: 0x%02x\n",rbyte);
  return(rbyte);
}

static inline void spiCMD(uint8_t command, uint16_t address)
{

  uint8_t b1 = (command<<1) | ((address & 0x100)>>8);
  uint8_t b2 = address & 0xff;

  printf("OUT: 0x%02x, 0x%02x\n",command, address);
  printf("Trans: 0x%02x 0x%02x\n",b1,b2);
  assertCS();
  
  // Byte 1 ... 3 bits of command + 1 bit of address
  (void)spi_xfer(SPI2,b1 );

  // Byte 2 ... 8 bits of address
  (void)spi_xfer(SPI2, b2);

  //(void)spi_read(SPI2); //Wait here till xfer is done
  while (SPI_SR(SPI2) & SPI_SR_BSY) ;

  deassertCS();
  return;
}

static inline void writeEnable()
{
  if (writeEnabled) return;
  spiCMD(0x4,0x180);
  writeEnabled=1;
  return;
}

EEPROM9366GLOBAL void eeprom9366_eraseAll()
{
  writeEnable();
  spiCMD(0x4, 0x100);
  delayms(10);
}

EEPROM9366GLOBAL void eeprom9366_erase(uint16_t address)
{
  writeEnable();
  spiCMD(0x7,address);
  delayms(10);
  return;
}


EEPROM9366GLOBAL void eeprom9366_write(uint16_t address, uint8_t data)
{
  writeEnable();
  (void)spiTransaction(0x5,address,data);
  delayms(10);
  return;
}


EEPROM9366GLOBAL uint8_t eeprom9366_read(uint16_t address)
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
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9); //Software Slave-Select
  gpio_clear(GPIOB,GPIO9);
  gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO9);
  
  gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN,
                  GPIO10 | GPIO14 | GPIO15);
  gpio_set_af(GPIOB, GPIO_AF5, GPIO10 | GPIO14 | GPIO15);
  gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
                                 GPIO10 | GPIO15); //SCK and MOSI are driven
  

  rcc_periph_clock_enable(RCC_SPI2);

  //spi_reset(SPI2);
  spi_init_master(SPI2,1000000,SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                   SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT,
                  SPI_CR1_MSBFIRST);
  
    //spi_set_dff_8bit(SPI2);
  spi_disable_crc(SPI2);
  spi_set_next_tx_from_buffer(SPI2);
  spi_set_full_duplex_mode(SPI2);
  //spi_send_msb_first(SPI2);
  //spi_set_baudrate_prescaler(SPI2,SPI_CR1_BAUDRATE_FPCLK_DIV_128);
  //spi_set_master_mode(SPI2);
  //spi_enable_ss_output(SPI2);
  spi_disable_tx_buffer_empty_interrupt(SPI2);
  spi_disable_rx_buffer_not_empty_interrupt(SPI2);
  spi_disable_error_interrupt(SPI2);
  //spi_set_standard_mode(SPI2,0);

  spi_set_baudrate_prescaler(SPI2,0x5); //Divide 48MHz Pclk by 64

  //spi_init_master(SPI2,SPI_CR1_BAUDRATE_FPCLK_DIV_128,0,0,SPI_CR1_DFF_8BIT,SPI_CR1_MSBFIRST);

  spi_enable(SPI2);
  return;
}

#ifdef TESTEEPROM
EEPROM9366GLOBAL void eeprom9366_test()
{
  uint8_t d;
  
  myprintf("EEProm Test\n");
  //eeprom9366_eraseAll();
  //myprintf("Erase complete\n");
  d = eeprom9366_read(0x10);
  myprintf("EEPROM read after erase: 0x%x\n",d);
  eeprom9366_write(0x10,0xde); delayms(10);
  d = eeprom9366_read(0x10);
  myprintf("EEPROM read after write (0xDE): 0x%x\n",d);
  //eeprom9366_erase(0x10);
  delayms(10);
  d = eeprom9366_read(0x10);
  myprintf("EEPROM read after cell erase: 0x%x\n",d);

  return;
}
#endif
