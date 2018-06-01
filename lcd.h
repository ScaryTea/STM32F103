#include "stm32f1xx.h"
#include "spi.h"

extern void display_clear();

extern void lcd_init();

extern void display_enable();

extern void display_disable();

extern void display_send_byte(uint8_t c);

extern void display_send_bytes(uint8_t* c, unsigned int n);




