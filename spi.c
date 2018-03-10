#include "stm32f1xx.h"

/*
Semisegment indicator, connection scheme
for shift register(pins Qa(1)-Qh(8))

   3
  ---
2| 1 |4
  --- 
8|   |6
  --- -5 
   7

*/

// digits '0-9', '.'
short digits[] = { 238, 40, 205, 109, 43, 103, 231, 44, 239, 111, 16 };
int currDigit = 0;

int spi_send(char byte);
void spi_init();

int main() {
	spi_init();
	spi_send(digits[currDigit]);

	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
	TIM2->PSC = 8000;
	TIM2->ARR = 1000;
	SET_BIT(TIM2->CCMR1, 0x6 << TIM_CCMR1_OC1M_Pos);
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	SET_BIT(TIM2->DIER, TIM_DIER_UIE);
	NVIC_EnableIRQ(TIM2_IRQn);
	
	while(1) ;
}

void TIM2_IRQHandler(void) {
	spi_send(digits[currDigit]);
	currDigit = currDigit < 9 ? currDigit + 1 : 0;
	TIM2->SR &= ~TIM_SR_UIF;
}

void spi_init() {
	//init port A and SPI1 
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN | 
		RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN);
	// 5 - Clock pin, 7 - Data pin
	MODIFY_REG(GPIOA->CRL,
		GPIO_CRL_CNF5 | GPIO_CRL_MODE5 | GPIO_CRL_CNF7 | GPIO_CRL_MODE7,
 		GPIO_CRL_CNF5_1|GPIO_CRL_MODE5_1 | GPIO_CRL_CNF7_1|GPIO_CRL_MODE7_1);
	// 3 - Latch pin
	MODIFY_REG(GPIOA->CRL,
		   GPIO_CRL_CNF3 | GPIO_CRL_MODE3,
		   GPIO_CRL_MODE3_1);
	// Transmit only mode
	CLEAR_BIT(SPI1->CR1, SPI_CR1_BIDIMODE | SPI_CR1_RXONLY);
	SET_BIT(SPI1->CR2, SPI_CR2_SSOE);
	SET_BIT(SPI1->CR1, SPI_CR1_MSTR);
}

int spi_send(char byte) {
	if(!(SPI1->SR&SPI_SR_TXE)) return 1;
	// SPI Enable
	SET_BIT(SPI1->CR1, SPI_CR1_SPE);
	// Latch off
	GPIOA->ODR &= ~GPIO_ODR_ODR3;
	SPI1->DR = byte;
	while(!(SPI1->SR&SPI_SR_TXE));
	// Latch on
	GPIOA->ODR |= GPIO_ODR_ODR3;
	// SPI Disable
	CLEAR_BIT(SPI1->CR1, SPI_CR1_SPE);
	return 0;
}



