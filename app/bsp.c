/*******************************************************************************
*           Copyright (C) 2015 Michael R. Ferrara, All rights reserved.
*
*                       Santa Rosa, CA 95404
*                       Tel:(707)536-1330
*
* Filename:     bsp.c
*
* Description: BSP "driver"
*
*******************************************************************************/
//#define TRACE_PRINT 1

#define GLOBAL_BSP
#include "bsp.h"
#include "OSandPlatform.h"

static const clock_scale_t clock32F411 = {
  //96 MHz, 16MHz crystal
  .pllm = 16, //1MHz reference
  .plln = 192, // 192MHz VCO
  .pllp = 2, // 96 MHz core clock
  .pllq = 4, // 48 MHz USB clock
  .hpre = RCC_CFGR_HPRE_DIV_NONE, // 96MHz AHB
  .ppre1 = RCC_CFGR_PPRE_DIV_2,  
  .ppre2 = RCC_CFGR_PPRE_DIV_2,
  .flash_config = FLASH_ACR_ICE | FLASH_ACR_DCE |
  FLASH_ACR_LATENCY_3WS,
  .apb1_frequency = 48000000,
  .apb2_frequency = 48000000,
};

void greenOn(int on)
{
  if (on) {
    gpio_set(GPIOB,GPIO0);
  } else {
    gpio_clear(GPIOB,GPIO0);
  }
  return;
}


void redOn(int on)
{
  if (on) {
    gpio_set(GPIOA,GPIO0);
  } else {
    gpio_clear(GPIOA,GPIO0);
  }
  return;
}

void hvOn(int on)
{
  if (on) {
    gpio_set(GPIOD,GPIO2);
    delayms(40); //Power-up time is about 40ms
  }  else {
    gpio_clear(GPIOD,GPIO2);
  }
}


void setupClocks(void)
{

  rcc_clock_setup_hse_3v3(&clock32F411);
  SystemCoreClock = 96000000;
  return;
}

void setupGPIOs(void)
{
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOD);

  // Setup USBOTG Clocking and pins
  rcc_periph_clock_enable(RCC_OTGFS);
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  GPIO9 | GPIO11 | GPIO12);
  gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);
  

  //Red LED
  gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
  gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO0);
  gpio_clear(GPIOA,GPIO0);
  
  //Grn LED
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
  gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO0);
  gpio_clear(GPIOB,GPIO0);
  
  //Control IOs
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0|GPIO1|
                  GPIO2|GPIO3|GPIO4|GPIO5|GPIO6|GPIO7|GPIO8|
                  GPIO9|GPIO10|GPIO11|GPIO12);
  gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO0|GPIO1|
                  GPIO2|GPIO3|GPIO4|GPIO5|GPIO6|GPIO7|GPIO8|
                  GPIO9|GPIO10|GPIO11|GPIO12);
  gpio_clear(GPIOC, GPIO0|GPIO1|
                  GPIO2|GPIO3|GPIO4|GPIO5|GPIO6|GPIO7|GPIO8|
                  GPIO9|GPIO10|GPIO11|GPIO12);
  
  //HV Enable
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);
  gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO2);
  gpio_clear(GPIOD, GPIO2);

  eeprom9366_init();
  return;
}

void setupNVIC(void)
{
  const uint32_t interrupt_priority_group4 = (0x3 << 8); // 15 priority interrupts, no sub-priorities
  scb_set_priority_grouping(interrupt_priority_group4);
  for (int irqNum=0; irqNum<=NVIC_IRQ_COUNT ; irqNum++) {
    nvic_set_priority(irqNum, 0x6f);
  }
  nvic_set_priority(-4,0); //MMU Fault
  nvic_set_priority(-5,0); //Bus Fault
  nvic_set_priority(-6,0); //Usage Fault
  nvic_set_priority(-11,0); //SVCall
  nvic_set_priority(-14,0); //PendSV
  nvic_set_priority(-15,0); //SysTick

  return;
}

void Delay(volatile uint32_t nCount)
{
  while (nCount--) {
    __asm__("nop");
  }
}
