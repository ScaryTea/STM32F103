#include "stm32f1xx.h"
#include "stdbool.h"

extern char* str;
extern bool is_busy;

extern void uart_init();

extern void send_byte(char byte);

extern bool uart_send(char* s);

extern void USART2_IRQHandler(void);
