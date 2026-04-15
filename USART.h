#ifndef USART_H
#define USART_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "stm32l476xx.h"
#include "queue.h"

void USART_Init (USART_TypeDef * USARTx);

void USART_Write_BaseType(USART_TypeDef * USARTx, BaseType_t * buffer);
void USART_Write(USART_TypeDef * USARTx, uint8_t * buffer, uint32_t nBytes);

uint32_t USART_Read(USART_TypeDef * USARTx, uint8_t * buffer, uint32_t nBytes);
uint8_t USART_ReadByte(USART_TypeDef * USARTx);

void USART1_IRQHandler();
void USART3_IRQHandler();
void USART2_IRQHandler();

extern QueueHandle_t SensorQueue;
extern QueueHandle_t frequency_queue;
extern QueueHandle_t volume_queue;
extern QueueHandle_t freq_peek_queue;
extern QueueHandle_t vol_peek_queue;

#endif 
