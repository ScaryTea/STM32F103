#include "stm32f1xx.h"
#include "uart.h"

#define MPU_ADDR 0b1101000

extern void I2C_init();
extern void I2C_write_byte(uint32_t reg_address, uint32_t data);
extern uint8_t I2C_read_byte(uint32_t reg_address);

