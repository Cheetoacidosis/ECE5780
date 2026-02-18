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




//void ADC1_Wakeup(void)
//{
//	int wait_time; 
//	
//	// To start ADC operations, the following sequence should be applied 
//	// DEEPPWD = 0: ADC not in deep-power down 
//	// DEEPPWD = 1: ADC in deep-power-down (default reset state) 
//	if((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD) {  
//		// Exit deep power down mode if still in that state 
//		ADC1->CR &= ~ADC_CR_DEEPPWD; 
//	}
//	
//	// Enable the ADC internal voltage regulator 
//	// Before performing any operation such as Launching a calibration or 
//	// enabling the ADC, the ADC voltage regulator must first be enabled and 
//	// the software must wait for the regulator start-up time. 
//	ADC1->CR |= ADC_CR_ADVREGEN; 
//	// Wait for ADC voltage requlator start-up time. The software must wait for
//	// the startup time of the ADC voltage regulator (T_ADCVREG_STUP, i.e., 20 
//	// us) before Launching a calibration or enabling the ADC. 
//	wait_time = 20 * (80000000 / 1000000); 
//	while (wait_time != 0) 
//	{ 
//		wait_time--; 
//	}
//}

//void ADC_Init(void)
//{
//	//Enable ADC clock
//	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

//	//Calibrate ADC
//	//Disable ADC for configuration
//	ADC1->CR &= ~ADC_CR_ADEN;
//	
//	//enable boost and vref
//	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;
//	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;
//	
//	//ckmode pclk/2 -> set bit 0 only
//	ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;
//	ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0;
//	
//	//Enable internal temperature sensor
//	ADC123_COMMON->CCR |= ADC_CCR_TSEN;
//	
//	//Wake up regulator
//	ADC1_Wakeup();
//	
//	//12 bit, right aligned 
//	ADC1->CFGR &= ~ADC_CFGR_RES;
//	ADC1->CFGR &= ~ADC_CFGR_ALIGN;
//	
//	//Regular seqence length = 0 (1 conversion); SQ1 = channel 17
//	//In STM32L4, SQ1 field is at bits 11:6, so << 6
//	ADC1->SQR1 &= ~ADC_SQR1_L;
//	ADC1->SQR1 &= ~ADC_SQR1_SQ1;
//	ADC1->SQR1 |= (17u << 6);		//no pos macro needed
//	
//	//Single ended on ch17
//	ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_17;
//	
//	//Sampling time for ch17, SMPR2 bits 23:21 (position 21
//	ADC1->SMPR2 &= ~(7u << 21);
//	ADC1->SMPR2 |= (7u << 21);		//247.5 cycles: safest for temp sensor
//	
//	//single conversion, software trigger
//	ADC1->CFGR &= ~ADC_CFGR_CONT;
//	ADC1->CFGR &= ~ADC_CFGR_EXTEN;
//		
//	//Enable ADC and wait ready
//	ADC1->CR |= ADC_CR_ADEN;
//	while(!(ADC1->ISR & ADC_ISR_ADRDY));
//}

//uint16_t ADC_TempSensorData(void)
//{
//	ADC1->CR |= ADC_CR_ADSTART;
//	while(!(ADC123_COMMON->CSR & ADC_CSR_EOC_MST));
//	
//	//scale value
//	float raw = ADC1->DR * (3.3/3);
//	
//	//convert to C
//	float temp = ((110-30)/3750)*raw + 30;
//	
//	return (uint16_t)temp;
//}
