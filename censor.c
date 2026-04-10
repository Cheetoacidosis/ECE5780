#include "censor.h"

//Kinda works in tandem with data.c, because we need UART tyvm

QueueHandle_t SensorQueue;
QueueHandle_t reading;

//Read the temp-er-at-ure
void xUS100SensorRead(){
	//Setup
	BaseType_t buffer;
	
	for (;;){
		//Wait indefinitely until data in queue
		xQueueReceive( SensorQueue,
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
			
			uint8_t message[] = {tens, ones, ' ', 'd', 'e', 'g', ' ', 'F', '\n', '\r'};
			
			USART_Write(USART2, message, sizeof(message));
		}
		else if (buffer == 'p') {
			uint8_t command = 0x55;
			USART_Write(USART1, &command, 1);
			
			BaseType_t dist[2] = {};
			xQueueReceive(reading, &dist[0], portMAX_DELAY);
			xQueueReceive(reading, &dist[1], portMAX_DELAY);	
			
//			uint8_t a = dist[0];
//			uint8_t b = dist[0] >> 8;
//			uint8_t c = dist[0] >> 16;
//			uint8_t d = dist[0] >> 24;
//			uint8_t A = dist[1];
//			
//			uint8_t new_dist[] = {a, A, 'A', '\n', '\r'};			
//			USART_Write(USART2, new_dist, sizeof(new_dist));
			
			float f_dist = dist[1] + (dist[0] << 8);
			f_dist /= 10;
			
			uint8_t hundreds = (uint8_t)(f_dist / 100);
			uint8_t tens = ((uint8_t)(f_dist / 10)) % 10;
			uint8_t ones = ((uint8_t)(f_dist)) % 10;
			
			hundreds += '0';
			tens += '0';
			ones += '0';
			
			uint8_t message[] = {hundreds, tens, ones, ' ', 'c', 'm', '\n', '\r'};
			
			USART_Write(USART2, message, sizeof(message));		
		}
	}
}