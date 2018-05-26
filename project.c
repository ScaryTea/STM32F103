#include <stdio.h>
#include "uart.h"
#include "i2c.h"
#include "spi.h"
#include "lcd.h"

void setup_pic_reciever();
unsigned char get_angle(int16_t angle_value, int8_t z);

void delayMil(int millis) {
	for (volatile int i = millis*1000; i > 0; i--);
}

int main() {
	I2C_init();	
	setup_pic_reciever();

	//SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	//MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0 | GPIO_CRL_CNF0, GPIO_CRL_MODE0_1);

	I2C_write_byte(0x6B, 0); // MPU power on
	unsigned char tab[] = {'|','\0'};
	unsigned char cr[] = {'\n','\r','\0'};
	unsigned char rotation[] = {'0','\0'}; 
	// 0 - 0, 1 - 180, 2 - 270, 3 - 90
	char c[18];
	unsigned char prev_angle = '0', curr_angle = '0';

	while(1) {
		delayMil(1500);
		uint8_t resH = I2C_read_byte(0x3D);
		uint8_t resL = I2C_read_byte(0x3E);
		int8_t resZ = I2C_read_byte(0x3F);
		curr_angle = get_angle((resH<<8)|resL,resZ);
		if(curr_angle != prev_angle); {
			rotation[0] = curr_angle;
			uart1_send(rotation);
			delayMil(50);
			uart1_send(tab);
		}
		prev_angle = curr_angle;
		int16_t ttt = (resH<<8)|resL;
		sprintf (c, "%d",ttt);
		uart1_send((unsigned char*)c);
		delayMil(50);
		uart1_send(tab);

		sprintf (c, "%d",resZ);
		uart1_send((unsigned char*)c);
		delayMil(50);
		uart1_send(tab);

		delayMil(50);
		uart1_send(cr);
		
	}
}

unsigned char get_angle(int16_t angle_value, int8_t z) {
	#define rot 8192
	if(angle_value >= -13000 && angle_value <= 13000 && z > 0) return '0';
	if(angle_value >= 13000 && angle_value <= 16384) return '1';
	if(angle_value >= -13000 && angle_value <= 13000 && z < 0) return '2';
	if(angle_value >= -16384 && angle_value <= -13000) return '3';
	return '0';
}

void setup_pic_reciever() {
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	uart1_init();
	spi_init();
	lcd_init();

	// enable DMA transmitter for SPI
	SET_BIT(SPI1->CR1, SPI_CR2_TXDMAEN);
	
	// enable DMA reciever for UART
	SET_BIT(USART1->CR3, USART_CR3_DMAR);

	// enable event generating for UART
//	SET_BIT(USART1->CR1, USART_CR1_RXNEIE);

	// set peripheral and memory addresses
	DMA1_Channel5->CPAR = (uintptr_t) &(USART1->DR);
	DMA1_Channel5->CMAR = (uintptr_t) &(SPI1->DR);

	// Priority - the highest (of course)
	DMA1_Channel5->CCR |= 3 << DMA_CCR_PL_Pos;
	
	// number of bytes to be transfered
	DMA1_Channel5->CNDTR = 100800;

	MODIFY_REG(DMA1_Channel5->CCR, 
		DMA_CCR_PSIZE | DMA_CCR_MSIZE | DMA_CCR_DIR,
		DMA_CCR_TCIE);

	// enable DMA channel 5
	SET_BIT(DMA1_Channel5->CCR, DMA_CCR_EN);

	display_enable();
}
