#ifndef LED_H_
#define LED_H_

#include <libopencm3/stm32/gpio.h>
#include "config.h"

void led_init(void);

#define led_backlight_on()     { }
#define led_backlight_off()    { }
#define led_backlight_toggle() { }

#define led_button_r_on()      {\
    gpio_clear(GPIOB, GPIO4);\
}
#define led_button_r_off()     {\
    gpio_set(GPIOB, GPIO4);\
}
#define led_button_r_toggle()  { \
    gpio_toggle(GPIOB, GPIO4);\
}

#define led_button_l_on()      {\
    gpio_clear(GPIOB, GPIO3);\
}
#define led_button_l_off()     {\
    gpio_set(GPIOB, GPIO3);\
}
#define led_button_l_toggle()  { \
    gpio_toggle(GPIOB, GPIO3);\
}

#endif  // LED_H_

