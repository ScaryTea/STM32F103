#include "stm32f1xx.h"

int main(void) {
	// enable tacting
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	//configuring portmodes
	GPIOA->CRL &= ~GPIO_CRL_MODE6;
	GPIOA->CRL = (GPIOA->CRL|GPIO_CRL_MODE5_1)&(~GPIO_CRL_MODE5_0);
	GPIOA->CRL &= ~GPIO_CRL_CNF5;
	GPIOA->CRL = (GPIOA->CRL|GPIO_CRL_CNF6_1)&(~GPIO_CRL_CNF6_0);
	GPIOA->ODR |= GPIO_ODR_ODR6;
	while(1) {
		if(!(GPIOA->IDR & GPIO_IDR_IDR6)){
			GPIOA->ODR &= ~GPIO_ODR_ODR5;
		}
		else {
			GPIOA->ODR |= GPIO_ODR_ODR5;
		}
		for (volatile int i = 0; i < 4000; i++);
	}
	return 0;
}
