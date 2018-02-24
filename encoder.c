#include "stm32f1xx.h"
#include "uart.c"
#include <stdlib.h>
#include <stdio.h>

int p4 = 0;
int p6 = 0;

int main() {
	// enable tacting
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

	// Button SW(PB0)
	MODIFY_REG(GPIOB->CRL,
			  GPIO_CRL_CNF0 | GPIO_CRL_MODE0,
			  GPIO_CRL_CNF0_1);
	SET_BIT(GPIOB->ODR, GPIO_ODR_ODR0); 

	// CLK(PA4) and DT(PA6) encoder outputs
	MODIFY_REG(GPIOA->CRL,
			  GPIO_CRL_CNF6 | GPIO_CRL_MODE6 | GPIO_CRL_CNF4 | GPIO_CRL_MODE4,
			  GPIO_CRL_CNF6_1 | GPIO_CRL_CNF4_1); 
	SET_BIT(GPIOA->ODR, GPIO_ODR_ODR6 | GPIO_ODR_ODR4);

	// Choose pins for EXTI lines	
 	SET_BIT(AFIO->EXTICR[1], AFIO_EXTICR2_EXTI6_PA | AFIO_EXTICR2_EXTI4_PA);
	SET_BIT(AFIO->EXTICR[0], AFIO_EXTICR1_EXTI0_PB);
	
	// Unmasking EXTI6 and EXTI4 and EXTI0 lines
	SET_BIT(EXTI->IMR, EXTI_IMR_MR6 | EXTI_IMR_MR4);
	SET_BIT(EXTI->IMR, EXTI_IMR_MR0 | EXTI_IMR_MR0);

	// Enabling falling and rising triggering on both pins
	SET_BIT(EXTI->FTSR, EXTI_FTSR_TR6 | EXTI_FTSR_TR4);
	SET_BIT(EXTI->RTSR, EXTI_RTSR_TR6 | EXTI_RTSR_TR4);
	SET_BIT(EXTI->RTSR, EXTI_RTSR_TR0);

	// Enabling NVIC handlers
	NVIC_EnableIRQ(EXTI4_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI0_IRQn);

	uart_init();	
	while(!uart_send("Hello!"));
	while(1);
}

void EXTI9_5_IRQHandler(void) {
	if(GPIOA->IDR & GPIO_IDR_IDR4) {
		p4 <<= 1;
		p4 |= 1;
	}
	else { 
		p4 <<= 1;
	}
	if(GPIOA->IDR & GPIO_IDR_IDR6) {
		p6 <<= 1;
		p6 |= 1;
	}
	else { 
		p6 <<= 1;
	}

	EXTI->PR |= EXTI_PR_PR6;
}

void EXTI4_IRQHandler(void) {
	if(GPIOA->IDR & GPIO_IDR_IDR4) {
		p4 <<= 1;
		p4 |= 1;
	}
	else {
		p4 <<= 1;
	}

	if(GPIOA->IDR & GPIO_IDR_IDR6) {
		p6 <<= 1;
		p6 |= 1;
	}
	else { 
		p6 <<= 1;
	}

	EXTI->PR |= EXTI_PR_PR4;
}

void EXTI0_IRQHandler(void) {
	char* s2 = (char*)malloc(20);
	sprintf(s2, "%d", p4);
	while(!uart_send(s2));
	free(s2);
	EXTI->PR |= EXTI_PR_PR0;
}
