#include "stm32f1xx.h"
#include "spi.h"

/*
first shift register - row selection
second - exact pixel
connect each pin with appropriate row/pixel,
e.g. connect pinout Q1 of first register to 
row_1 pin of led matrix and so on.
*/

int currCol = 0;
char rowStates[8];

void set_pixel(unsigned int x, unsigned int y) {
	if(y < 0 || y > 7 || x < 0 || x > 7) return;
	rowStates[y] |= 1 << (7 - x);	
}

void draw_vline(unsigned int x) {
	if(x > 7) return;
	for(int i = 0; i < 8; i++) {
		rowStates[i] |= 1 << (7 - x);
	}
}

void draw_hline(unsigned int y) {
	if(y > 7) return;
	rowStates[y] = 255;
}

char r4, r5;
int main() {
	spi_init();

	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
	TIM2->PSC = 200;
	TIM2->ARR = 60;
	SET_BIT(TIM2->CCMR1, 0x6 << TIM_CCMR1_OC1M_Pos);
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	SET_BIT(TIM2->DIER, TIM_DIER_UIE);
	NVIC_EnableIRQ(TIM2_IRQn);
	for(volatile int i = 0; i < 8; i++) rowStates[i] = 0;
	spi_send(255);
	spi_send(0);
	draw_vline(5);
	draw_hline(6);
	while(1) ;
}

void TIM2_IRQHandler(void) {
	// spi_send(rowStates[currCol]);
	// spi_send(~(1 << currCol));
	currCol = currCol < 7 ? currCol + 1 : 0;
	TIM2->SR &= ~TIM_SR_UIF;
}



