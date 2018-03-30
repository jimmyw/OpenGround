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

static u8g2_t u8g2;
static volatile int ready = 0;

static uint8_t u8x8_gpio_and_delay_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO10);
      gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO11);
      // Init 
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
  u8g2_Setup_ssd1306_i2c_128x64_noname_2(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_i2c);

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

