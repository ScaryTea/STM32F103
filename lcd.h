#include "stm32f1xx.h"
#include "spi.h"

/*
Pins:
CLK - A5 (clk)
DIN - A7 (data)
DC - A1 (data/command)
SCE - A3 ("latch"), active LOW
RST - A2, active LOW
*/

extern void display_clear();

extern void lcd_init();

extern void display_enable();

extern void display_disable();

extern void display_send_byte(uint8_t c);

extern void display_send_bytes(uint8_t* c, unsigned int n);

extern void test();




