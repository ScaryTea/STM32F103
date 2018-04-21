#include "uart.h"
#include "spi.h"
#include "lcd.h"
// sudo socat OPEN:/home/vlad_ko/test /dev/ttyUSB0,b9600,raw,echo=0

int main() {

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
	SET_BIT(USART1->CR1, USART_CR1_RXNEIE);

	// set peripheral and memory addresses
	DMA1_Channel5->CPAR = (uintptr_t) &(USART1->DR);
	DMA1_Channel5->CMAR = (uintptr_t) &(SPI1->DR);
	
	// number of bytes to be transfered
	DMA1_Channel5->CNDTR = 1008;

	MODIFY_REG(DMA1_Channel5->CCR, 
		DMA_CCR_PSIZE | DMA_CCR_MSIZE | DMA_CCR_DIR,
		DMA_CCR_TCIE);

	// enable DMA channel 5
	SET_BIT(DMA1_Channel5->CCR, DMA_CCR_EN);

	display_enable();
	//spi_send(0b11111111);

	//MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE4|GPIO_CRL_CNF4, GPIO_CRL_MODE4_1);
	//MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0|GPIO_CRL_CNF0, GPIO_CRL_MODE0_1);
	while(1) {}
}
