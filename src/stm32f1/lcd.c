#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/i2c.h>
#include "lcd.h"
#include "u8g2.h"
#include "adc.h"
#include "stringutil.h"
#include "debug.h"

static u8g2_t u8g2;
static volatile int ready = 0;

static uint8_t u8x8_byte_stm32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t *data;
  uint32_t reg32 __attribute__((unused));

 
  switch(msg)
  {
    case U8X8_MSG_BYTE_SEND:
      data = (uint8_t *)arg_ptr;
      debug("hw_i2c send: ");
      debug_put_int8(arg_int);
      debug(" bytes\n");

      while( arg_int > 0 )
      {
        i2c_send_data(I2C2, *data);
        while (!(I2C_SR1(I2C2) & I2C_SR1_BTF)) {
            if (I2C_SR1(I2C2) & I2C_SR1_BTF)
                debug("BTF\n");
            if (I2C_SR1(I2C2) & I2C_SR1_TxE)
                debug("TxE\n");
            if (I2C_SR1(I2C2) & I2C_SR1_ADDR)
                debug("ADDR\n");
            if (I2C_SR1(I2C2) & I2C_SR1_SB)
                debug("SB\n");

        }
        //while (!(I2C_SR1(I2C2) & (I2C_SR1_BTF | I2C_SR1_TxE)));
        data++;
        arg_int--;
      }
      break;
    case U8X8_MSG_BYTE_INIT:
      debug("hw_i2c init\n");
      rcc_periph_clock_enable(RCC_GPIOB);
      rcc_periph_clock_enable(RCC_I2C2);
      rcc_periph_clock_enable(RCC_AFIO);
      gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
		      GPIO_I2C2_SCL | GPIO_I2C2_SDA);
      i2c_peripheral_disable(I2C2);
      i2c_set_clock_frequency(I2C2, I2C_CR2_FREQ_42MHZ);
      //i2c_set_fast_mode(I2C2);
      i2c_set_standard_mode(I2C2);
      //i2c_set_ccr(I2C2, 0x1e);
      //i2c_set_trise(I2C2, 0x0b);
      i2c_set_ccr(I2C2, 35);
      i2c_set_trise(I2C2,43);
      i2c_set_own_7bit_slave_address(I2C2, 0x00);
      i2c_peripheral_enable(I2C2);
      break;
      
    case U8X8_MSG_BYTE_SET_DC:
      break;
      
    case U8X8_MSG_BYTE_START_TRANSFER: {
      uint8_t addr = u8x8_GetI2CAddress(u8x8);
      debug("hw_i2c start addr: ");
      debug_put_hex8(addr);
      debug("\n");
      i2c_send_start(I2C2);
      while (!((I2C_SR1(I2C2) & I2C_SR1_SB)
		& (I2C_SR2(I2C2) & (I2C_SR2_MSL | I2C_SR2_BUSY))));
      
      i2c_send_7bit_address(I2C2, addr, I2C_WRITE);
      while (!(I2C_SR1(I2C2) & I2C_SR1_ADDR));
      reg32 = I2C_SR2(I2C2);
      break;
    }
    case U8X8_MSG_BYTE_END_TRANSFER:
      debug("hw_i2c end\n");
      i2c_send_stop(I2C2);
      break;
    default:
      return 0;
  }
  return 1;
}

static uint8_t u8x8_gpio_and_delay_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    return 1;
}


static uint8_t u8x8_gpio_and_delay_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      debug("sw_i2c init\n");
      gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO10);
      gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO11);
      break;
    case U8X8_MSG_DELAY_NANO:
      /* not required for SW I2C */
      break;
    
    case U8X8_MSG_DELAY_10MICRO:
      /* not used at the moment */
      break;
    
    case U8X8_MSG_DELAY_100NANO:
      /* not used at the moment */
      break;
   
    case U8X8_MSG_DELAY_MILLI:
      //for(int i = 0; i < 1000; i++) __asm__("nop");
      break;
    case U8X8_MSG_DELAY_I2C:
      /* delay by 5 us when arg_int is 1, or 1.25us otherwise */
      //if (arg_int)
      //  for(int i = 0; i < 10; i++) __asm__("nop");
      //else
      //  for(int i = 0; i < 2; i++) __asm__("nop");
      break;
    
    case U8X8_MSG_GPIO_I2C_CLOCK:
      
      if ( arg_int == 0 )
      {
        gpio_clear(GPIOB, GPIO10);
      }
      else
      {
        gpio_set(GPIOB, GPIO10);
      }
      break;
    case U8X8_MSG_GPIO_I2C_DATA:
      
      if ( arg_int == 0 )
      {
        gpio_clear(GPIOB, GPIO11);
      }
      else
      {
        gpio_set(GPIOB, GPIO11);
      }
      break;
    default:
      break;
  }
  return 1;
}

void lcd_init(void) {
  u8g2_Setup_ssd1306_i2c_128x64_noname_2(&u8g2, U8G2_R0, u8x8_byte_stm32_hw_spi, u8x8_gpio_and_delay_hw_i2c);
  //u8g2_Setup_ssd1306_i2c_128x64_noname_2(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_i2c);  

  u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
  u8g2_SetPowerSave(&u8g2, 0); // wake up display
  u8g2_ClearBuffer(&u8g2);
  u8g2_FirstPage(&u8g2);
  do {
      u8g2_SetFont(&u8g2, u8g2_font_amstrad_cpc_extended_8f);
      u8g2_DrawStr(&u8g2, 0,10, "Hello world");
  } while ( u8g2_NextPage(&u8g2) );
  ready = 1;
}

int screen = 0;
void lcd_send_data(const uint8_t *x) {
  if (!ready)
      return;
  char buf[32];
  // Render screen
  u8g2_FirstPage(&u8g2);
  do {
    //u8g2_DrawLine(&u8g2, 64-j, 64-j, j, j);
    switch (screen) {
        case 0:
            u8g2_DrawCircle(&u8g2, 32 + (-adc_get_channel_rescaled(1) * 32 / 0xe00), 32 + (-adc_get_channel_rescaled(0) * 32 / 0xe00), 2, U8G2_DRAW_ALL);
            u8g2_DrawCircle(&u8g2, 96 + (-adc_get_channel_rescaled(3) * 32 / 0xe00), 32 + (-adc_get_channel_rescaled(2) * 32 / 0xe00), 2, U8G2_DRAW_ALL);
            break;
        case 1:
            for (int i = 0; i < 4; i++) {
              int2bufhex(adc_get_channel(i), buf);
              u8g2_DrawStr(&u8g2, 0, (i * 10) + 10, buf);
              int2bufhex(adc_get_channel_rescaled(i), buf);
              u8g2_DrawStr(&u8g2, 50, (i * 10) + 10, buf);
              u8g2_DrawLine(&u8g2, 0, (i * 2) + 57, 64 + ((adc_get_channel_rescaled(i) * 64) / 0xe00), (i * 2) + 57); // 128 px wide screen
            }
            break;
     }
  } while ( u8g2_NextPage(&u8g2) );

}

void lcd_powerdown(void) {}
void lcd_show_logo(void) {}

