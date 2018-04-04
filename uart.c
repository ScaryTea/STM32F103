#include "uart.h"

unsigned char* str;

void uart1_init();
void uart2_init();
void uart3_init();
void uart1_send(unsigned char* s);
void uart2_send(unsigned char* s);
void uart3_send(unsigned char* s);

void uart1_init() {
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
	// tx - AFIO push-pull
	MODIFY_REG(GPIOA->CRH, 
		GPIO_CRH_CNF9_0 | GPIO_CRH_MODE9_0,
		GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_1);
	// rx - input floating
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF10 | GPIO_CRH_MODE10, GPIO_CRH_CNF10_0);
	// baudrate
	USART1->BRR = 0x341;
	// Update event bit, transmit|recieve enable 
	SET_BIT(USART1->CR1, USART_CR1_UE | USART_CR1_RE | USART_CR1_TE);
}

void uart2_init() {
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);
	// tx - AFIO push-pull
	MODIFY_REG(GPIOA->CRL, 
		GPIO_CRL_CNF2_0 | GPIO_CRL_MODE2_0,
		GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2_1);
	// rx - input floating
	MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF3 | GPIO_CRL_MODE3, GPIO_CRL_CNF3_0);
	// baudrate
	USART2->BRR = 0x341;
	// Update event bit, transmit|recieve enable 
	SET_BIT(USART2->CR1, USART_CR1_UE | USART_CR1_RE | USART_CR1_TE);
}

void uart3_init() {
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART3EN);
	// tx - AFIO push-pull
	MODIFY_REG(GPIOB->CRH, 
		GPIO_CRH_CNF10_0 | GPIO_CRH_MODE10_0,
		GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_1);
	// rx - input floating
	MODIFY_REG(GPIOB->CRH, GPIO_CRH_CNF11 | GPIO_CRH_MODE11, GPIO_CRH_CNF11_0);
	// baudrate
	USART3->BRR = 0x341;
	// Update event bit, transmit|recieve enable 
	SET_BIT(USART3->CR1, USART_CR1_UE | USART_CR1_RE | USART_CR1_TE);
}

unsigned char uart1_get() {
	while(!(USART1->SR&USART_SR_RXNE));
	unsigned char c = USART1->DR;
	return c;
}

unsigned char uart2_get() {
	while(!(USART2->SR&USART_SR_RXNE));
	unsigned char c = USART2->DR;
	return c;
}

unsigned char uart3_get() {
	while(!(USART3->SR&USART_SR_RXNE));
	unsigned char c = USART3->DR;
	return c;
}

void send_byte_u1(unsigned char byte) {
	if(byte != '\0') {
		USART1->DR = byte;
		SET_BIT(USART1->CR1, USART_CR1_TXEIE);
	}
	else {
		CLEAR_BIT(USART1->CR1, USART_CR1_TXEIE);
	}
}

void send_byte_u2(unsigned char byte) {
	if(byte != '\0') {
		USART2->DR = byte;
		SET_BIT(USART2->CR1, USART_CR1_TXEIE);
	}
	else {
		CLEAR_BIT(USART2->CR1, USART_CR1_TXEIE);
	}
}

void send_byte_u3(unsigned char byte) {
	if(byte != '\0') {
		USART3->DR = byte;
		SET_BIT(USART3->CR1, USART_CR1_TXEIE);
	}
	else {
		CLEAR_BIT(USART3->CR1, USART_CR1_TXEIE);
	}
}

void uart1_send(unsigned char* s) {
	if(!(USART1->SR&USART_SR_TXE));
	str = s;
	send_byte_u1(*s);
}

void uart2_send(unsigned char* s) {
	if(!(USART2->SR&USART_SR_TXE));
	str = s;
	send_byte_u2(*s);
}

void uart3_send(unsigned char* s) {
	if(!(USART3->SR&USART_SR_TXE));
	str = s;
	send_byte_u3(*s);
}

void USART1_IRQHandler(void) {
	send_byte_u1(*(++str));
}

void USART2_IRQHandler(void) {
	send_byte_u2(*(++str));
}

void USART3_IRQHandler(void) {
	send_byte_u3(*(++str));
}
