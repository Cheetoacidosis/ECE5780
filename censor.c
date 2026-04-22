#include "censor.h"
#include "USART.h"

//Kinda works in tandem with data.c, because we need UART tyvm

QueueHandle_t SensorQueue;
QueueHandle_t frequency_queue;
QueueHandle_t volume_queue;
QueueHandle_t freq_peek_queue;
QueueHandle_t vol_peek_queue;



//void vFrequencyTask() {
//    while(1) {
//        uint8_t command = 0x55;
//        USART_Write(USART1, &command, 1);

//        vTaskDelay(pdMS_TO_TICKS(50)); // 20 Hz sampling
//    }
//}



//void vVolumeTask() {
//    while(1) {
//        uint8_t command = 0x55;
//        USART_Write(USART3, &command, 1);

//        vTaskDelay(pdMS_TO_TICKS(50));
//    }
//}



////Read the temp-er-at-ure
void xUS100SensorRead(){
	//Setup
	//BaseType_t buffer;
	const BaseType_t UART_WAIT_TIME = 150;
	float f_dist = 0;
	uint16_t dist_freq = 0;
	uint16_t dist_vol = 0;
	uint16_t old_ASS_CNT = 0;
	uint16_t old_volume = 0;
	
	for (;;){

		
		// Poll Freq sensor
		// Save its value
		// Poll Volume sensor
		// Save its value
		
		// Check if values match old values
		// If different, write to UART2
// Frequency Sensor	
		// Empty the queue before beginning
			BaseType_t qReturn0 = pdPASS;
			BaseType_t empty = 0;
			while (qReturn0 != errQUEUE_EMPTY) {
				 qReturn0 = xQueueReceive(frequency_queue, &empty, 0);
			}
			
			uint8_t command = 0x55;
			USART_Write(USART1, &command, 1);
			
			BaseType_t dist[2] = {};
			BaseType_t qReturn1 = xQueueReceive(frequency_queue, &dist[0], portTICK_PERIOD_MS*UART_WAIT_TIME);
			BaseType_t qReturn2 = xQueueReceive(frequency_queue, &dist[1], portTICK_PERIOD_MS*UART_WAIT_TIME);
			
			// if both queues returned successfully
			if ((qReturn1 != errQUEUE_EMPTY)){// && (qReturn2 != errQUEUE_EMPTY)){
					f_dist = dist[1] + (dist[0] << 8);
					f_dist /= 10;
						
					dist_freq = (uint16_t) f_dist;
					xQueueOverwrite(freq_peek_queue, &dist_freq);
					
					//uint8_t message[] = {'f','r','e','q','\n','\r'};
					//USART_Write(USART2, message, sizeof(message));
					
			// Assume that the reading was too far away
			} else {
					dist_freq = 100;
				  xQueueOverwrite(freq_peek_queue, &dist_freq);
					//uint8_t message[] = {'O', 'O', 'B', '\n','\r'};
					//USART_Write(USART2, message, sizeof(message));	
			}	
		
// Volume Sensor
			// Empty the queue before beginning
			qReturn0 = pdPASS;
			empty = 0;
			while (qReturn0 != errQUEUE_EMPTY) {
				 qReturn0 = xQueueReceive(volume_queue, &empty, 0);
			}
			
			command = 0x55;
			USART_Write(USART3, &command, 1);
			
			dist[0] = 0; dist[1] = 0;
			qReturn1 = xQueueReceive(volume_queue, &dist[0], portTICK_PERIOD_MS*UART_WAIT_TIME);
			qReturn2 = xQueueReceive(volume_queue, &dist[1], portTICK_PERIOD_MS*UART_WAIT_TIME);
			
			// if both queues returned successfully
			if ((qReturn1 != errQUEUE_EMPTY)){// && (qReturn2 != errQUEUE_EMPTY)){
					f_dist = 0;
					f_dist = dist[1] + (dist[0] << 8);
					f_dist /= 10;
						
					dist_vol = (uint16_t) f_dist;
					xQueueOverwrite(vol_peek_queue, &dist_vol);
					
					//uint8_t message[] = {'v','o','l','u','\n','\r'};
					//USART_Write(USART2, message, sizeof(message));
					
			// Assume that the reading was too far away
			} else {
					dist_vol = 100;
				  xQueueOverwrite(vol_peek_queue, &dist_vol);
					//uint8_t message[] = {'O', 'O', 'B', '\n','\r'};
					//USART_Write(USART2, message, sizeof(message));	
			}	
		

// Re-create the math that is done to these values
		// Frequency
		uint16_t ASS_CNT = ((uint16_t)dist_freq - 5);
		if (ASS_CNT < 0) {
			ASS_CNT = 0;
		}
		else if (ASS_CNT >= 25) {   
			ASS_CNT = 24;
		}
			
		// Volume
		if (dist_vol > 50) dist_vol = 50; // clamp
		if (dist_vol < 0) dist_vol = 0;

		// Scale to 0–7.9 range
		float scale = 8 * dist_vol / 50.0;
		// Chop off decimal point
		uint16_t volume = (uint16_t)scale;
		
		
// Check if either value changed from its old
		if (old_ASS_CNT != ASS_CNT){
					uint8_t message[] = {'f','r','e','q',' ','c','h','g','d', '\n','\r'};
					USART_Write(USART2, message, sizeof(message));	
		}
		if (old_volume != volume){
					uint8_t message[] = {'V','o','l',' ',(volume + '0'), '\n','\r'};
					USART_Write(USART2, message, sizeof(message));	
		}
		
		
		
// Update the "old" values
		old_ASS_CNT = ASS_CNT;
		old_volume = volume;
		
	}
}		







		//Wait indefinitely until data in queue
/* ck4.21		xQueueReceive( SensorQueue,
			&buffer,
			portMAX_DELAY);
		
		//When pulled out of blocked state, send UART command to sensor (0x50)
		if (buffer == 't') {
			// Declare variables
			uint8_t hundreds = 0;
			uint8_t tens = 0;
			uint8_t ones = 0;
			
			// Empty the queue before beginning
			BaseType_t qReturn0 = pdPASS;
			BaseType_t empty = 0;
			while (qReturn0 != errQUEUE_EMPTY) {
				 qReturn0 = xQueueReceive(frequency_queue, &empty, 0);
			}
			
			uint8_t command = 0x55;
			USART_Write(USART1, &command, 1);
			
			BaseType_t dist[2] = {};
			BaseType_t qReturn1 = xQueueReceive(frequency_queue, &dist[0], portTICK_PERIOD_MS*UART_WAIT_TIME);
			BaseType_t qReturn2 = xQueueReceive(frequency_queue, &dist[1], portTICK_PERIOD_MS*UART_WAIT_TIME);
			
			// if both queues returned successfully
			if ((qReturn1 != errQUEUE_EMPTY) && (qReturn2 != errQUEUE_EMPTY)){
					float f_dist = dist[1] + (dist[0] << 8);
					f_dist /= 10;
						
					uint16_t dist_as_int = (uint16_t) f_dist;
					xQueueOverwrite(freq_peek_queue, &dist_as_int);
					
					hundreds = (uint8_t)(f_dist / 100);
					tens = ((uint8_t)(f_dist / 10)) % 10;
					ones = ((uint8_t)(f_dist)) % 10;
					
					hundreds += '0';
					tens += '0';
					ones += '0';
					
					// Bandaid hundreds-being-too-big bug
					if (hundreds > '9') hundreds = '9';
					
					uint8_t message[] = {hundreds, tens, ones, ' ', 'c', 'm', '\n', '\r'};
					USART_Write(USART2, message, sizeof(message));
					
			// Assume that the reading was too far away
			} else {
					uint16_t dist_as_int = 100;
				  xQueueOverwrite(freq_peek_queue, &dist_as_int);
					uint8_t message[] = {'O', 'O', 'B', '\n','\r'};
					USART_Write(USART2, message, sizeof(message));	
			}	
			
		}
		else if (buffer == 'p') {
			// Empty the queue before beginning
			BaseType_t qReturn0 = pdPASS;
			BaseType_t empty = 0;
			while (qReturn0 != errQUEUE_EMPTY) {
				 qReturn0 = xQueueReceive(volume_queue, &empty, 0);
			}
			
			uint8_t command = 0x55;
			USART_Write(USART3, &command, 1);
			
			BaseType_t dist[2] = {};
			BaseType_t qReturn1 = xQueueReceive(volume_queue, &dist[0], portTICK_PERIOD_MS*UART_WAIT_TIME);
			BaseType_t qReturn2 = xQueueReceive(volume_queue, &dist[1], portTICK_PERIOD_MS*UART_WAIT_TIME);
			
				//only write if the uart returned in time
			if ((qReturn1 != errQUEUE_EMPTY) && (qReturn2 != errQUEUE_EMPTY)){
					float f_dist = dist[1] + (dist[0] << 8);
					f_dist /= 10;
						
					uint16_t dist_as_int = (uint16_t) f_dist;
					xQueueOverwrite(vol_peek_queue, &dist_as_int);
					
					uint8_t hundreds = (uint8_t)(f_dist / 100);
					uint8_t tens = ((uint8_t)(f_dist / 10)) % 10;
					uint8_t ones = ((uint8_t)(f_dist)) % 10;
					
					hundreds += '0';
					tens += '0';
					ones += '0';
					
					uint8_t message[] = {hundreds, tens, ones, ' ', 'c', 'm', '\n', '\r'};
					
					USART_Write(USART2, message, sizeof(message));		
			}
		} ck4.21 */ 





