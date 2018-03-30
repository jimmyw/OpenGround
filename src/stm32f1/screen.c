#include "screen.h"
#include "lcd.h"
void screen_init(void) {}
void screen_clear(void) {}
void screen_update(void) {
    lcd_send_data(0);
}
void screen_test(void) {}

