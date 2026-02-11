#include "censor.h"

//Kinda works in tandem with data.c, because we need UART tyvm

QueueHandle_t SensorQueueueueueueueueueueueueueueue;

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
		uint8_t command = 0x50;
		USART_Write(USART1, &command, 1);
		//
	
	}
}



void USART1_IRQHandler(){
	uint8_t buffer[sizeof(BaseType_t)] = {};
	BaseType_t pRx_counter = 0;
	
	if(USART1->ISR & USART_ISR_RXNE) { // Received data
		buffer[pRx_counter] = USART1->RDR;
		// Reading USART_DR automatically clears the RXNE flag
		(pRx_counter)++;
		if((pRx_counter) >= 1 )
			(pRx_counter) = 0;
	}
	
	//Parse the data
	//Eventually this will add the value to a queue, and defer parsing to a task
	
}