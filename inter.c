#include "stm32f1xx.h"

int main() {
	// enable tacting
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	GPIOC->CRH &= ~GPIO_CRH_CNF13;
	GPIOC->CRH = (GPIOC->CRH|GPIO_CRH_MODE13_1)&(~GPIO_CRH_MODE13_0);
        GPIOA->CRL = (GPIOA->CRL|GPIO_CRL_CNF6_1)&(~GPIO_CRL_CNF6_0);

        GPIOA->ODR |= GPIO_ODR_ODR6;

	AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PA;

	EXTI->IMR |= EXTI_IMR_MR6;
	EXTI->FTSR |= EXTI_FTSR_TR6;

	NVIC_EnableIRQ(EXTI9_5_IRQn);
	/*
	uart_init();
	char * s = "Hello";
	uart_send(s); */

// Usart starts here
	GPIOA->CRL = (GPIOA->CRL|GPIO_CRL_CNF2_1)&(~GPIO_CRL_CNF2_0);
        GPIOA->CRL = (GPIOA->CRL|GPIO_CRL_MODE2_1)&(~GPIO_CRL_MODE2_0);

	//USART->CR1 &= ~USART->CR1_M;

	USART2->BRR = 0x341;

	USART2->CR1 |= USART_CR1_UE;

	USART2->CR1 |= USART_CR1_TE;

	USART2->DR = 't';

	while(!(USART2->SR&USART_SR_TXE));

	 USART2->DR = 'a';

// Ends here

	while(1) { for(volatile int i = 0; i < 5000000; i++); }
}

void EXTI9_5_IRQHandler(void) {
	GPIOC->ODR ^= GPIO_ODR_ODR13;
	EXTI->PR |= EXTI_PR_PR6;
}
