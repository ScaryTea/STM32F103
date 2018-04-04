#include "spi.h"

void spi_init() {
	//init port A and SPI1 
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN | 
		RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN);
	// 5 - Clock pin, 7 - Data pin
	MODIFY_REG(GPIOA->CRL,
		GPIO_CRL_CNF5 | GPIO_CRL_MODE5 | GPIO_CRL_CNF7 | GPIO_CRL_MODE7,
 		GPIO_CRL_CNF5_1|GPIO_CRL_MODE5_1 | GPIO_CRL_CNF7_1|GPIO_CRL_MODE7_1);
	// Transmit only mode
	CLEAR_BIT(SPI1->CR1, SPI_CR1_BIDIMODE | SPI_CR1_RXONLY);
	// so slave select pin can be used as gpio
	SET_BIT(SPI1->CR2, SPI_CR2_SSOE);
	// master mode
	SET_BIT(SPI1->CR1, SPI_CR1_MSTR);
	CLEAR_BIT(SPI1->CR1, SPI_CR1_LSBFIRST | SPI_CR1_DFF);
	SET_BIT(SPI1->CR1, 0x4 << SPI_CR1_BR_Pos);
	// enable spi
	SET_BIT(SPI1->CR1, SPI_CR1_SPE);
	CLEAR_BIT(SPI1->CR1, SPI_CR1_CPOL | SPI_CR1_CPHA);
}

void disable_spi() {
	CLEAR_BIT(SPI1->CR1, SPI_CR1_SPE);
	CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
}

void spi_send(char byte) {
	while (!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = byte;
	while ((SPI1->SR & SPI_SR_BSY));
}



