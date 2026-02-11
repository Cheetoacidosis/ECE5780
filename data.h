#ifndef __STM32L476R_NUCLEO_DATA_H
#define __STM32L476R_NUCLEO_DATA_H
#include "stm32l476xx.h"
#include <stdint.h>

void USART_Init (USART_TypeDef * USARTx);
void USART_Write(USART_TypeDef * USARTx, uint8_t * buffer, uint32_t nBytes);
uint32_t USART_Read(USART_TypeDef * USARTx, uint8_t * buffer, uint32_t nBytes);
uint8_t USART_ReadByte(USART_TypeDef * USARTx);
//void ADC1_Wakeup(void);
//void ADC_Init(void);
//uint16_t ADC_TempSensorData(void);

#endif