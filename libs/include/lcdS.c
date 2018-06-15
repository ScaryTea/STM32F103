#include "stm32f103xb.h"
#include <stdio.h>
#include "stm32f1xx.h"
void switchLED(void);
void writeToUSART3(char);
void writeMultipleToUSART3(const char*);
void spi1_send_8(char);
void spi1_send_16(uint16_t);
void spi2_send_8(char);
void display_try_do_something(void);
void displey_set_dc(void);
void display_reset_dc(void);
void display_clear(void);
void display_enable(void);
void display_disable(void);
void display_initialize(void);
int curRow = 0;
char matrix[] = {
    0b00011010,
    0b00100110,
    0b01000010,
    0b11111111,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01111110,
};
int main(void)
{
        // Tacting
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

//DISPLAY START
    //SPI 2:
    //NSS = B12
    //SCK = B13
    //MISO = B14
    //MOSI = B15
    //DC TO A8
    //RST TO A9
    //SCE TO A10

    // MOSI and SCK to af push-pull, 2MHz, rest not needed
    GPIOB->CRH = (GPIOB->CRH | GPIO_CRH_CNF15_1 | GPIO_CRH_MODE15_1)
        & ~(GPIO_CRH_CNF15_0 | GPIO_CRH_MODE15_0);
    GPIOB->CRH = (GPIOB->CRH | GPIO_CRH_CNF13_1 | GPIO_CRH_MODE13_1)
        & ~(GPIO_CRH_CNF13_0 | GPIO_CRH_MODE13_0);
    //DC, RST gen purpose push-pull 2 MHz
    GPIOA->CRH = (GPIOA->CRH | GPIO_CRH_MODE8_1)
        & ~(GPIO_CRH_CNF8 | GPIO_CRH_MODE8_0);
    GPIOA->CRH = (GPIOA->CRH | GPIO_CRH_MODE9_1)
        & ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9_0);
    GPIOA->CRH = (GPIOA->CRH | GPIO_CRH_MODE10_1)
        & ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10_0);
    
    
    //reset on-off
    GPIOA->ODR &= ~GPIO_ODR_ODR9;
    for (volatile int i = 0; i < 10000; ++i);
    GPIOA->ODR |= GPIO_ODR_ODR9;
    GPIOA->ODR |= GPIO_ODR_ODR10;
    //baud rate = clk/32
    SPI2->CR1 |= (0b100 << SPI_CR1_BR_Pos);
    //SPI2->CR1 &= ~(SPI_CR1_BR);
    //CPOL
    //CPHA
    //8 bit
    SPI2->CR1 &= ~SPI_CR1_DFF;
    SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
    //SSM
    //SSI
    SPI2->CR2 |= SPI_CR2_SSOE;
    SPI2->CR1 |= SPI_CR1_MSTR;
    SPI2->CR1 |= SPI_CR1_SPE;
    
    display_try_do_something();
//DISPLAY END
    while(1) {
    }
}
void display_set_dc() {
    GPIOA->ODR |= GPIO_ODR_ODR8;
}
void display_reset_dc() {
    GPIOA->ODR &= ~GPIO_ODR_ODR8;
}
void display_enable() {
    GPIOA->ODR &= ~GPIO_ODR_ODR10;
}
void display_disable() {
    GPIOA->ODR |= GPIO_ODR_ODR10;
}
void display_initialize() {
    display_enable();
    
    display_reset_dc();
    //extended instruction set, powerup
    spi2_send_8(0b00100001);
    //set bias, optimal is 4
    spi2_send_8(0x14);
    //set Vop
    spi2_send_8(0b10111100); //black magic  // visibility in 0111000 - 1001111
    //normal instruction set
    spi2_send_8(0b00100000);
    //display - normal mode
    spi2_send_8(0b00001100);
}
void display_clear() {
    for (volatile int y = 0; y <= 5; ++y) {
        display_reset_dc();
        //set y address     
        spi2_send_8(0b01000000 | y);
        //set x address to 0
        spi2_send_8(0b10000000);
        display_set_dc();
        for (volatile int x = 0; x <= 83; ++x) {
            spi2_send_8(0);
        }
    }
}
void display_try_do_something() {
    display_initialize();
    display_clear();
    display_set_dc();
    //data send
    spi2_send_8(0b00011111);
    spi2_send_8(0b00000100);
    spi2_send_8(0b00011111);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00010101);
    spi2_send_8(0b00010101);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00010000);
    spi2_send_8(0b00010000);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00010000);
    spi2_send_8(0b00010000);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00010001);
    spi2_send_8(0b00011111);
    spi2_send_8(0);
    spi2_send_8(0);
    spi2_send_8(0);
    spi2_send_8(0);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00001000);
    spi2_send_8(0b00011111);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00010001);
    spi2_send_8(0b00011111);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00001101);
    spi2_send_8(0b00010111);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00010000);
    spi2_send_8(0b00010000);
    spi2_send_8(0);
    spi2_send_8(0b00011111);
    spi2_send_8(0b00010001);
    spi2_send_8(0b00001110);
    spi2_send_8(0);
    spi2_send_8(0b00010111);
    display_disable();
}

void spi2_send_8(char byte) {
    while (!(SPI2->SR & SPI_SR_TXE));
    SPI2->DR = byte;
    while ((SPI2->SR & SPI_SR_BSY));
}

