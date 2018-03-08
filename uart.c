#include "uart.h"

char* str;
bool is_busy = false;

void uart_init() {
	//init uart2 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	GPIOA->CRL = (GPIOA->CRL|GPIO_CRL_CNF2_1|GPIO_CRL_MODE2_1)
				 &(~GPIO_CRL_CNF2_0)&(~GPIO_CRL_MODE2_0);
	USART2->BRR = 0x341;
	USART2->CR1 |= USART_CR1_UE;
	USART2->CR1 |= USART_CR1_TE;
	NVIC_EnableIRQ(USART2_IRQn);
}

void send_byte(char byte) {
	if(byte != '\0') {
		USART2->DR = byte;
		SET_BIT(USART2->CR1, USART_CR1_TXEIE);
	}
	else {
		CLEAR_BIT(USART2->CR1, USART_CR1_TXEIE);
		is_busy = false;
	}
}

bool uart_send(char* s) {
	if(is_busy) return false;
	is_busy = true;
	str = s;
	send_byte(*s);
	return true;
}

void USART2_IRQHandler(void) {
	send_byte(*(++str));
}

/*
	// init uart1
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	GPIOA->CRH = (GPIOA->CRL|GPIO_CRH_CNF9_1|GPIO_CRH_MODE9_1)
			&(~GPIO_CRH_CNF9_0)&(~GPIO_CRH_MODE9_0);
	USART1->BRR = 0x341;
	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_TE;
	NVIC_EnableIRQ(USART1_IRQn);
	//init uart2 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	GPIOA->CRL = (GPIOA->CRL|GPIO_CRL_CNF2_1|GPIO_CRL_MODE2_1)
			&(~GPIO_CRL_CNF2_0)&(~GPIO_CRL_MODE2_0);
	USART2->BRR = 0x341;
	USART2->CR1 |= USART_CR1_UE;
	USART2->CR1 |= USART_CR1_TE;
	NVIC_EnableIRQ(USART2_IRQn);
	// init uart3 
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	GPIOB->CRL = (GPIOB->CRL|GPIO_CRL_CNF10_1|GPIO_CRL_MODE10_1)
			&(~GPIO_CRL_CNF10_0)&(~GPIO_CRL_MODE10_0);
	USART3->BRR = 0x341;
	USART3->CR1 |= USART_CR1_UE;
	USART3->CR1 |= USART_CR1_TE;
	NVIC_EnableIRQ(USART3_IRQn);
*/
