#include "i2c.h"

void I2C_init();
void I2C_write_byte(uint32_t reg_address, uint32_t data);
uint8_t I2C_read_byte(uint32_t reg_address);
void start();
void send_addr(uint8_t rw);
void send_data(uint32_t data);
void stop();
void ack_config(unsigned int on_off);

void I2C_write_byte(uint32_t reg_address, uint32_t data) {
	start();
	send_addr(0);
	send_data(reg_address);
	send_data(data);
	stop();
}

uint8_t I2C_read_byte(uint32_t reg_address) {
	start();
	send_addr(0);
	send_data(reg_address);	
	start();	
	send_addr(1);
	ack_config(1);
	while(!(I2C1->SR1 & I2C_SR1_RXNE));
	uint8_t res = I2C1->DR;
	ack_config(0);
	stop();
	return res;
}

void start() {
	SET_BIT(I2C1->CR1, I2C_CR1_START);
	while (!(I2C1->SR1 & I2C_SR1_SB)); // start cond generated
}

void send_addr(uint8_t rw) { 
	/* rw: 0 -> write; 1 -> read */
	I2C1->DR = (MPU_ADDR << 1) | rw;
	while(!(I2C1->SR1 & I2C_SR1_ADDR)); // address matched
	// read SR2 to clear ADDR
	if(!(I2C1->SR2 & I2C_SR2_TRA));
}

void send_data(uint32_t data) {
	while (!(I2C1->SR1 & I2C_SR1_TXE)); // DR empty
	I2C1->DR = data;
	while (!(I2C1->SR1 & I2C_SR1_TXE));
}

void stop() {
	SET_BIT(I2C1->CR1, I2C_CR1_STOP);
	while(I2C1->SR1 & I2C_SR2_MSL);
}

void ack_config(unsigned int on_off) {
	if(on_off)
		SET_BIT(I2C1->CR1, I2C_CR1_ACK);
	else
		CLEAR_BIT(I2C1->CR1, I2C_CR1_ACK);
}

void I2C_init() {
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	// alternate function open-drain
	MODIFY_REG(GPIOB->CRL,
			 GPIO_CRL_MODE6 | GPIO_CRL_MODE7,
			 GPIO_CRL_CNF6 | GPIO_CRL_CNF7 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1);
	
	// I2C configuration 
	CLEAR_BIT(I2C1->CCR, I2C_CCR_DUTY);
	SET_BIT(I2C1->OAR1, I2C_OAR1_ADD1_7);
	SET_BIT(I2C1->CR1, I2C_CR1_ACK);
	SET_BIT(I2C1->CR2, 0x08 << I2C_CR2_FREQ_Pos); // 8 MHz peripheral clock
	SET_BIT(I2C1->TRISE, 0x09); // rise time = 1000 ns
	SET_BIT(I2C1->CCR, 0x28); // Q = 100 kHz
	SET_BIT(I2C1->CR1, I2C_CR1_PE); // periph enable
}
