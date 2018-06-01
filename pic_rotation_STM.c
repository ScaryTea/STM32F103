#include "uart.h"
#include "i2c.h"
#include "spi.h"
#include "lcd.h"

void setup_dma_reciever();
unsigned char get_angle(int16_t angle_value, int8_t z);
static void delay(int millis);

int main() {
	I2C_init();
	setup_dma_reciever();

	I2C_write_byte(0x6B, 0); // MPU power on

	// rotation values: 0 - 0, 1 - 180, 2 - 270, 3 - 90
	unsigned char rotation[] = {'?','\0'}; 
	unsigned char prev_angle = '?', curr_angle = '0';

	while(1) {
		uint8_t resH = I2C_read_byte(0x3D); // ACCEL_YH
		uint8_t resL = I2C_read_byte(0x3E); // ACCEL_YL
		int8_t resZ = I2C_read_byte(0x3F);  // ACCEL_ZH
		curr_angle = get_angle((resH<<8)|resL,resZ);
		if(curr_angle != prev_angle) {
			rotation[0] = curr_angle;
			uart1_send(rotation);
		}
		prev_angle = curr_angle;	
		delay(500);	
	}
}

unsigned char get_angle(int16_t angle_value, int8_t z) {
	#define rot 13000
	if(angle_value >= -rot && angle_value <= rot && z > 0) return '2';
	if(angle_value >= rot) return '3';
	if(angle_value >= -rot && angle_value <= rot && z < 0) return '0';
	if(angle_value <= -rot) return '1';
	return '0';
}

void setup_dma_reciever() {
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	uart1_init();
	spi_init();
	lcd_init();

	// enable DMA transmitter for SPI
	SET_BIT(SPI1->CR1, SPI_CR2_TXDMAEN);
	
	// enable DMA reciever for UART
	SET_BIT(USART1->CR3, USART_CR3_DMAR);

	// set peripheral and memory addresses
	DMA1_Channel5->CPAR = (uintptr_t) &(USART1->DR);
	DMA1_Channel5->CMAR = (uintptr_t) &(SPI1->DR);

	// priority - the highest (of course)
	DMA1_Channel5->CCR |= 3 << DMA_CCR_PL_Pos;
	
	// number of bytes to be transfered
	DMA1_Channel5->CNDTR = 504;

	// circular mode, number of bytes reloads
	DMA1_Channel5->CCR |= DMA_CCR_CIRC;

	MODIFY_REG(DMA1_Channel5->CCR, 
		DMA_CCR_PSIZE | DMA_CCR_MSIZE | DMA_CCR_DIR,
		DMA_CCR_TCIE);

	// enable DMA channel 5
	SET_BIT(DMA1_Channel5->CCR, DMA_CCR_EN);

	display_enable();
}

static void delay(int millis) {
	for (volatile int i = millis*1000; i > 0; i--);
}
