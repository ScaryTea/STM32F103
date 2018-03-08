#include "stm32f1xx.h"

int main() {
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	MODIFY_REG(GPIOA->CRL,
			   GPIO_CRL_CNF0 | GPIO_CRL_MODE0,
			   GPIO_CRL_CNF0_1 | GPIO_CRL_MODE0_1);

	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
	TIM2->PSC = 3;
	TIM2->ARR = 99;
	TIM2->CCR1 = 30;
	TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;
	TIM2->CCMR1 |= 0x6 << TIM_CCMR1_OC1M_Pos;
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
	TIM2->CR1 |= TIM_CR1_ARPE;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->CCER |= TIM_CCER_CC1E;

	while(1) 
	{
		for(volatile int i = 0; i < 100; i++) {
			TIM2->CCR1 = i;
			for(volatile int y = 0; y < 1000; y++);
		}
		for(volatile int i = 99; i > 0; i--) {
			TIM2->CCR1 = i;
			for(volatile int y = 0; y < 1000; y++);
		}
	} 
}
