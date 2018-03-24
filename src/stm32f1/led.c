
#include "led.h"
#include "delay.h"
#include "debug.h"

void led_init(void) {
    debug("led: init\n"); debug_flush();
	rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_GPIOB);

    // Turn off jtag mode
    gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON,0);
    gpio_set(GPIOB, GPIO3);
    gpio_set(GPIOB, GPIO4);

    // Enable output on the leds 
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO3);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
    
    // Just do a intro blink
    for(int i = 0; i < 4; i++) {
        delay_us(100000);
        gpio_toggle(GPIOB, GPIO3);
    }
    for(int i = 0; i < 4; i++) {
        delay_us(100000);
        gpio_toggle(GPIOB, GPIO4);
    }
}

