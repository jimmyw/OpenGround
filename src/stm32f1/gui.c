#include <stdint.h>
#include "gui.h"
#include "lcd.h"
void gui_init(void) {}
void gui_loop(void) {
    while(1) {
        lcd_send_data(0);
    }
}

uint32_t gui_running(void) { return 0; }

