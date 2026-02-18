#include "FreeRTOSConfig.h"
#include "LEDtoggle.h"
#include "queue.h"
#include "data.h"
#include "censor.h"

int main( void )
{
	BaseType_t queue_state = SetupQueue();
	/* Perform any hardware setup necessary. */
	prvSetupHardware();
	
	//enable interrupts
	NVIC_SetPriority(USART2_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
	NVIC_EnableIRQ(USART2_IRQn);
	USART_Init(USART2);
	
	//enable interrupts
	NVIC_SetPriority(USART1_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
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
		
		xTaskCreate( xUS100SensorRead,
									"Do a little UART1'ing",
									configMINIMAL_STACK_SIZE,
									NULL,
									2,
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
