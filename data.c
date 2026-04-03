#include "data.h"
#include "FreeRTOS.h"

void USART_Init(USART_TypeDef * USARTx) 
{  
	
	if (USARTx == USART2) {
		//Set up for USART2
		GPIOA->MODER &= ~(0xf << (4));
		GPIOA->MODER |= (0xa << (4));
		
		GPIOA->AFR[0] |= (0x77 << (8));
		
		GPIOA->OSPEEDR |= (0xf << 4);
		
		GPIOA->PUPDR &= ~(0xf << 4);
		GPIOA->PUPDR |= (0x5 << 4);
		
		GPIOA->OTYPER &= ~(0x3 << 2);
		
		RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
		
		RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
		RCC->CCIPR |= RCC_CCIPR_USART2SEL_0;
	
		USART2->CR1 |= USART_CR1_RXNEIE;
		USART2->CR1 &= ~USART_CR1_TXEIE;

	} 
	//Setup for USART1
	else if (USARTx == USART1) {
	//PA  
		//Tx 9 
		//Rx 10
		// put them into AF mode
		GPIOA -> MODER &= ~(0b11 << (9*2));
		GPIOA -> MODER &= ~(0b11 << (10*2));
		GPIOA -> MODER |= (0b10 << (9*2));
		GPIOA -> MODER |= (0b10 << (10*2));
		
		//Configure AF mode. 7 is USART rx/tx
		GPIOA->AFR[1] |= (0x77 << (4*1));
		
		//VERY HIGH SPEEEEEEED
		GPIOA->OSPEEDR |= (0xf << 2*9);
		
		//Pull=up
		GPIOA->PUPDR &= ~(0xf << 2*9);
		GPIOA->PUPDR |= (0x5 << 2*9);
		
		//Push-pull
		GPIOA->OTYPER &= ~(0x3 << 9);
		
		//Clock!!!
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		
		RCC->CCIPR &= ~RCC_CCIPR_USART1SEL;
		RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;

		USART1->CR1 |= USART_CR1_RXNEIE;
		USART1->CR1 &= ~USART_CR1_TXEIE;
	}


	
	// Disable USART 
	USARTx->CR1 &= ~USART_CR1_UE; 
	
	// Set data Length to 8 bits 
	// 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits 
	USARTx->CR1 &= ~USART_CR1_M;
	
	// Select 1 stop bit 
	// 00 = 1 stop bit 01 = 0.s stop bit 
	// 10 = 2 Stop bits 11 = 1.5 Stop bit 
	USARTx->CR2 &= ~USART_CR2_STOP; 
	
	// Set parity control as no parity 
	// 0 = no parity, 
	// 1 = parity enabled (then, program PS bit to select Even or Odd parity) 
	USARTx->CR1 &= ~USART_CR1_PCE;
	
	// Oversampling by 16 
	// 0 = oversampling by 16, 1 = oversampling by 8 
	USARTx->CR1 &= ~USART_CR1_OVER8;
	
	// Set Baud rate to 9600 using 4 MHz frequency 
	// See Example 1 in Section 22.1.2 
	// USARTx->BRR = 0x683; 
	USARTx->BRR = 0x1A1; 
	
	// Enable transmission and reception 
	USARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE); 
	
	// Enable USART 
	USARTx->CR1 |= USART_CR1_UE; 
	
	// Verify that USART is ready for transmission 
	// TEACK: Transmit enable acknowledge flag. Hardware sets or resets it. 
	while ((USARTx->ISR & USART_ISR_TEACK) == 0); 
	
	// Verify that USART is ready for reception 
	// REACK: Receive enable acknowledge flag. Hardware sets or resets it. 
	while ((USARTx->ISR & USART_ISR_REACK) == 0);
}



void USART_Write_BaseType(USART_TypeDef * USARTx, BaseType_t * buffer) {
	uint8_t buff_array[4] = {(uint8_t)*buffer&0xF000, (uint8_t)*buffer&0x0F00, (uint8_t)*buffer&0x00F0, (uint8_t)*buffer&0x000F};
	for (int i=0; i<4; i++) {
		USART_Write(USARTx, &buff_array[i], 1);
	}

}

void USART_Write(USART_TypeDef * USARTx, uint8_t * buffer, uint32_t nBytes)
{
	uint32_t i;
	
	for(i = 0; i < nBytes; i++)
	{
		// wait until TXE (TX empty) is set
		// writing USART_DR automatically clears the TXE flag
		while(!(USARTx->ISR & USART_ISR_TXE));
		USARTx->TDR = (buffer[i] & 0xFF);
	}
	
	while(!(USARTx->ISR & USART_ISR_TC));			//wait until TC bit is set
	USARTx->ICR = USART_ICR_TCCF;
}

uint32_t USART_Read(USART_TypeDef * USARTx, uint8_t * buffer, uint32_t nBytes)
{
	uint32_t i;
	
	for(i = 0; i < nBytes; i++) buffer[i] = USART_ReadByte(USARTx);
	
	return nBytes;
}

uint8_t USART_ReadByte(USART_TypeDef * USARTx)
{
	while(!(USARTx->ISR & USART_ISR_RXNE));
	return (uint32_t)USARTx->RDR;
}




