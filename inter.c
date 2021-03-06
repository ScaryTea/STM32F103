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

	while(1);
}

void EXTI9_5_IRQHandler(void) {
	GPIOC->ODR ^= GPIO_ODR_ODR13;
	EXTI->PR |= EXTI_PR_PR6;
}
