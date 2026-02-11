#include "data.h"

void USART_Init(USART_TypeDef * USARTx) 
{  
	// Enable GPIO clock and configure the Tx pin and the Rx pin as: 
	// Alternate function, High Speed, Push-pull, Pull-up 
	
	//------------------- GPIO Initialization for USART 2 ----------------- 
	// PB.6 = AF7 (USARTl_TX), PB.7 = AF7 (USARTl_RX), See Appendix I 
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable GPIO port A clock 
	
	// 00 = Input, 01 = Output, 10 = Alternate Function, 11 = Analog 
	GPIOA->MODER &= ~(0xF << (2*2)); // Clear mode bits for pin 6 and 7 
	GPIOA->MODER |= 0xA << (2*2); // Select Alternate Function mode
	
	// Alternative function 7 = USART 2 
	// Appendix I shows alL alternate functions 
//	GPIOA->AFR[0] |= 0x77 << (4*6); // Set pin 6 and 7 to AF 7 
	GPIOA->AFR[0] &= ~((0xFu << (2*4)) | (0xFu << (3*4)));
	GPIOA->AFR[0] |= ((7u << (2*4)) | (7u << (3*4)));
	// GPIO Speed: 00 = Low speed, 01 = Medium speed, 
	// 10 = Fast speed, 11 = High speed 
	GPIOA->OSPEEDR |= 0xF << (2*2); 
	
	// GPIO Push-Pull: 00 = No pull-uplpuLl-down, 01 = Pull-up (01) 
	// 10 = Pull-down, 11 = Reserved 
	GPIOA->PUPDR &= ~(0xF << (2*2)); 
	GPIOA->PUPDR |= 0x5 << (2*2); // Select pull-up 
	
	// GPIO Output Type: 0 = push-pull, 1 = open drain 
	GPIOA->OTYPER &= ~(0x3 << 2); 
	

	
	//Enable USART
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	
	//Select USART2
	RCC->CCIPR &= ~(3u << 2);
	RCC->CCIPR |= (1u << 2);
	
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