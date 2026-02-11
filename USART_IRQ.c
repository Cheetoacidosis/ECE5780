#include "stm32l476xx.h"

#define BufferSize 32

uint8_t USART1_Buffer_Rx[BufferSize], USART2_Buffer_Rx[BufferSize];
uint32_t Rx1_Counter = 0, Rx2_Counter = 0;

void USART_Write(USART_TypeDef * USARTx, uint8_t * buffer, int nBytes);
void USART_Init(USART_TypeDef * USARTx);

int main(void) {
	//enable GPIO clock and configure Tx pin and the Rx pin as:
	//alternate function, high speed, push-pull, pull-up
	
	RCC->AHB2ENR |=RCC_AHB2ENR_GPIOAEN;
	
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
	
	//enable interrupts
	USART2->CR1 |= USART_CR1_RXNEIE;
	USART2->CR1 &= ~USART_CR1_TXEIE;
	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_EnableIRQ(USART2_IRQn);
	
	USART_Init(USART2);
	
	uint8_t send[2] = {0x74, 0x76};
	
	USART_Write(USART2, send, 2);
	
	while(1);
}


void USART_Write(USART_TypeDef * USARTx, uint8_t * buffer, int nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
	// wait until TXE (TX empty) is set
	// Writing USART_DR automatically clears the TXE flag

		while (!(USART2->ISR & USART_ISR_TXE));
		USARTx->TDR = (buffer[i] & 0xFF);
	}
	while (!(USARTx->ISR & USART_ISR_TC)); // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}


void USART_IRQHandler(USART_TypeDef * USARTx, uint8_t * buffer, uint32_t * pRx_counter){
	if(USARTx->ISR & USART_ISR_RXNE) { // Received data
		buffer[*pRx_counter] = USARTx->RDR;
		// Reading USART_DR automatically clears the RXNE flag
		(*pRx_counter)++;
		if((*pRx_counter) >= BufferSize )
			(*pRx_counter) = 0;
	}
}

void USART2_IRQHandler(void) {
	USART_IRQHandler(USART2, USART2_Buffer_Rx, &Rx2_Counter);

	if (USART2_Buffer_Rx[Rx2_Counter - 1] == 0x74 || USART2_Buffer_Rx[Rx2_Counter - 1] == 0x54){
		uint8_t buffer[] = {'t', 'e', 'm', 'p', ' ', 'u', 'n', 'a', 'v', 'a', 'i', 'l', 'a', 'b', 'l', 'e', '\n', 13};
		USART_Write(USART2, buffer, 18);
	}
}

void USART_Init(USART_TypeDef * USARTx) {
	//disable USART
	USARTx->CR1 &= ~USART_CR1_UE;
	
	//set data length to 8 bits
	USARTx->CR2 &= ~USART_CR2_STOP;
	
	//set parity to none
	USARTx->CR1 &= ~USART_CR1_PCE;
	
	//oversampling by 16
	USARTx->CR1 &= ~USART_CR1_OVER8;
	
	//set baud rate to 9600
	USARTx->BRR = 0x1A1;
	
	//enable trensmission and reception
	USARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	
	//enable USART
	USARTx->CR1 |= USART_CR1_UE;
	
	//wait for USART ready for transmission
	while((USARTx->ISR & USART_ISR_TEACK) == 0);
	
	//wait for USART ready for reception
	while((USARTx->ISR & USART_ISR_REACK) == 0);
}
