#ifndef LED_H_
#define LED_H_

#include <libopencm3/stm32/gpio.h>
#include "config.h"

void led_init(void);

#define led_backlight_on()     { }
#define led_backlight_off()    { }
#define led_backlight_toggle() { }

#define led_button_r_on()      { }
#define led_button_r_off()     { }
#define led_button_r_toggle()  { }

#define led_button_l_on()      { }
#define led_button_l_off()     { }
#define led_button_l_toggle()  { }

#endif  // LED_H_

