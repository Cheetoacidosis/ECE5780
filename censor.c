#include "censor.h"

//Kinda works in tandem with data.c, because we need UART tyvm

QueueHandle_t SensorQueueueueueueueueueueueueueueue;
QueueHandle_t reading;

//Read the temp-er-at-ure
void xUS100SensorRead(){
	//Setup
	BaseType_t buffer;
	
	for (;;){
		//Wait indefinitely until data in queue
		xQueueReceive( SensorQueueueueueueueueueueueueueueue,
			&buffer,
			portMAX_DELAY);
		
		//When pulled out of blocked state, send UART command to sensor (0x50)
		if (buffer == 't') {
			uint8_t command = 0x50;
			USART_Write(USART1, &command, 1);
			
			BaseType_t temp = 0;
			xQueueReceive(reading, &temp, portMAX_DELAY);
			
			uint8_t tens = temp / 10;
			uint8_t ones = temp % 10;
			
			tens += '0';
			ones += '0';
			
			uint8_t message[] = {tens, ones, ' ', 'd', 'e', 'g', ' ', 'F', '\n'};
			
			USART_Write(USART2, message, sizeof(message));
		}
		else if (buffer == 'p') {
			uint8_t command = 0x55;
			USART_Write(USART1, &command, 1);
			
			BaseType_t dist = 0;
			xQueueReceive(reading, &dist, portMAX_DELAY);
			
			//float f_dist = (float) dist;
			dist *= 10;
			dist /= 2.54;
			
			uint8_t hundreds = dist / 1000;
			uint8_t tens = ((uint8_t)(dist / 100)) % 10;
			uint8_t ones = ((uint8_t) dist / 10) % 10;
			
			hundreds += '0';
			tens += '0';
			ones += '0';
			
			uint8_t message[] = {hundreds, tens, ones, ' ', 'i', 'n', 'c', 'h', 'e', 's', '\n'};
			
			USART_Write(USART2, message, sizeof(message));			
		}
	
	}
}



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
	//USART_Write(USART2, buffer, 2);
	
	//Parse the data
	//Eventually this will add the value to a queue, and defer parsing to a task
	
}