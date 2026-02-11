#include "FreeRTOSConfig.h"
#include "LEDtoggle.h"
#include "queue.h"
#include "data.h"

int main( void )
{
	BaseType_t queue_state = SetupQueue();
	/* Perform any hardware setup necessary. */
	prvSetupHardware();
	
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
	
	//enable interrupts
	USART1->CR1 |= USART_CR1_RXNEIE;
	USART1->CR1 &= ~USART_CR1_TXEIE;
	
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);
	USART_Init(USART1);
	
	if (queue_state != NULL) {
		/* --- APPLICATION TASKS CAN BE CREATED HERE --- */
		TaskHandle_t xHandle = NULL;
		
		xTaskCreate( LEDtoggle,
									"LED",
									configMINIMAL_STACK_SIZE,
									NULL,
									1,
									&xHandle
								);

		xTaskCreate( bouncyBoi,
									"Button Debounce",
									configMINIMAL_STACK_SIZE,
									NULL,
									1,
									&xHandle
								);
			
		/* Start the created tasks running. */
		vTaskStartScheduler();
	}
//	else {
//		GPIOA->ODR |= 1UL<<5;
//	}
	
	/* Execution will only reach here if there was insufficient heap
	to start the scheduler. */
	for( ;; );
	return 0;
}