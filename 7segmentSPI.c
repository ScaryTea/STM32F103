#include "stm32f1xx.h"
#include "spi.h"

/*
Seven-segment indicator, connection scheme
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

void send_char(char byte) {
	// Latch off
	GPIOA->ODR &= ~GPIO_ODR_ODR3;
	spi_send(digits[currDigit]);
	// Latch on
	GPIOA->ODR |= GPIO_ODR_ODR3;	
}

int main() {
	spi_init();
	MODIFY_REG(GPIOA->CRL,
		GPIO_CRL_CNF3 | GPIO_CRL_MODE3,
		GPIO_CRL_MODE3_1);
	
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

// just shows digits from 0 to 9 every second
void TIM2_IRQHandler(void) {
	send_char(digits[currDigit]);
	currDigit = currDigit < 9 ? currDigit + 1 : 0;
	TIM2->SR &= ~TIM_SR_UIF;
}
