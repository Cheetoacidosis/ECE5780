#include "USART.h"
#include "censor.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "data.h"

void USART1_IRQHandler(){
	uint8_t buffer[2] = {};
	BaseType_t pRx_counter = 0;
		
	for (int i=0; i < 2; i++) {
		if(USART1->ISR & USART_ISR_RXNE) { // Received data
			buffer[pRx_counter] = USART1->RDR;
			// Reading USART_DR automatically clears the RXNE flag
			(pRx_counter)++;
			if((pRx_counter) >= 2) {
				//(pRx_counter) = 0;
			}
		}
	}

	BaseType_t lengthened_buffer = (BaseType_t) ((buffer[0] & 0x00FF) + ((buffer[1] << 8) & 0xFF00));
	xQueueSendToBackFromISR(reading, &lengthened_buffer, NULL);

	//Parse the data
	//Eventually this will add the value to a queue, and defer parsing to a task
}


void USART3_IRQHandler(){
	//Copied from USART1 handler
	uint8_t buffer[2] = {};
	BaseType_t pRx_counter = 0;
		
	for (int i=0; i < 2; i++) {
		if(USART3->ISR & USART_ISR_RXNE) { // Received data
			buffer[pRx_counter] = USART3->RDR;
			// Reading USART_DR automatically clears the RXNE flag
			(pRx_counter)++;
			if((pRx_counter) >= 2) {
				//(pRx_counter) = 0;
			}
		}
	}
	
	BaseType_t lengthened_buffer = (BaseType_t) ((buffer[0] & 0x00FF) + ((buffer[1] << 8) & 0xFF00));
	xQueueSendToBackFromISR(reading_USART3, &lengthened_buffer, NULL);
	
}
