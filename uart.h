#include "stm32f1xx.h"

extern void uart1_init();
extern void uart2_init();
extern void uart3_init();

extern unsigned char uart1_get();
extern unsigned char uart2_get();
extern unsigned char uart3_get();

extern void uart1_send(unsigned char* s);
extern void uart2_send(unsigned char* s);
extern void uart3_send(unsigned char* s);

