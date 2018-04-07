#include "lcd.h"

/*
Pins:
CLK - A5 (clk)
DIN - A7 (data)
DC - A1 (data/command)
SCE - A3 ("latch"), active LOW
RST - A2, active LOW
*/

void delay(int millis) {
	for(volatile int i = 0; i < millis*1000; i++);
}

void display_data() {
    GPIOA->ODR |= GPIO_ODR_ODR1;
}

void display_command() {
    GPIOA->ODR &= ~GPIO_ODR_ODR1;
}

void display_enable() {
    GPIOA->ODR &= ~GPIO_ODR_ODR3;
}

void display_disable() {
    GPIOA->ODR |= GPIO_ODR_ODR3;
}

void display_clear() {
	for (int y = 0; y <= 5; ++y) {
		display_command();
		//set y address     
		spi_send(0b01000000 | y);
		//set x address to 0
		spi_send(0b10000000);
		display_data();
		for (int x = 0; x <= 83; ++x) {
		    	spi_send(0x0);
			delay(2);
		}
    }
}

void lcd_init() {
	int del = 10;

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	// RST and DC output push-pull 2MHz
	MODIFY_REG(GPIOA->CRL, 
		GPIO_CRL_MODE2 | GPIO_CRL_CNF2 | GPIO_CRL_MODE1 | GPIO_CRL_CNF1,
		GPIO_CRL_MODE2_1 | GPIO_CRL_MODE1_1);
	
	// SCE output push-pull 2MHz
	MODIFY_REG(GPIOA->CRL,
	   GPIO_CRL_CNF3 | GPIO_CRL_MODE3 | GPIO_CRL_CNF0 | GPIO_CRL_MODE0,
	   GPIO_CRL_MODE3_1 | GPIO_CRL_MODE0_1);
	
	// RST off
	GPIOA->ODR &= ~GPIO_ODR_ODR2;
	delay(del);
	// RST on
	GPIOA->ODR |= GPIO_ODR_ODR2;
	// SCE on (no clocks handling)
	GPIOA->ODR |= GPIO_ODR_ODR3;
	delay(del);
	spi_init();
	display_enable();
	display_command();
	// function set (extended instruction set)
	spi_send(0b00100001);
	// ???
	spi_send(0x14);
	// set Vop
	spi_send(0b10111100);
	// function set (std instruction set)
	spi_send(0b00100000);
	// display conf (normal mode)
	spi_send(0b00001100); 
	display_data();
	display_clear();
}

void display_send_byte(uint8_t c) {
	display_enable();
	spi_send(c);
	delay(3);
	display_disable();
}

void display_send_bytes(uint8_t* c, unsigned int n) {
	display_enable();
	for(int i = 0; i < n; i++) {
		spi_send(c[i]);
		delay(3);
	}
	display_disable();
}



void test() {
	
	lcd_init();
	uint8_t c[504];
	int i;
	for(i = 0; i < 504; i++)
	{
		c[i] = (uint8_t)i;
		//display_send_byte(0b10101010);
	}
	display_send_bytes(c,i);
	while(1);
}




