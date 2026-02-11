#include "LEDtoggle.h"
#include "queue.h"
#include "data.h"

//BaseType_t LEDstate = 0;
QueueHandle_t xStateQueue;

//Sine lookup table
const BaseType_t SLUT[64] = {
		0x800, 0x8c8, 0x98f, 0xa52, 0xb0f, 0xbc5, 0xc71, 0xd12,
		0xda7, 0xe2e, 0xea6, 0xf0d, 0xf63, 0xfa7, 0xfd8, 0xff5,
		0xfff, 0xff5, 0xfd8, 0xfa7, 0xf63, 0xf0d, 0xea6, 0xe2e,
		0xda7, 0xd12, 0xc71, 0xbc5, 0xb0f, 0xa52, 0x98f, 0x8c8,
		0x800, 0x737, 0x670, 0x5ad, 0x4f0, 0x43a, 0x38e, 0x2ed,
		0x258, 0x1d1, 0x159, 0xf2, 0x9c, 0x58, 0x27, 0xa,
		0x0, 0xa, 0x27, 0x58, 0x9c, 0xf2, 0x159, 0x1d1,
		0x258, 0x2ed, 0x38e, 0x43a, 0x4f0, 0x5ad, 0x670, 0x737};

// Note lookup table. First is high a, then b... 
const BaseType_t ARR_LUT[8] = {
		0x11B, //A220
		0x0fc,
		0x0ee,
		//0x0de,
		0x0d4,
		0x0bd,
		0x0b2,
		0x09e, //A440
		0x08d };

//Sine lookup table Counter
static BaseType_t SLUT_CNT = 0;
static BaseType_t ASS_CNT = 0;
		
//Sets up button, LED & TIM4
void prvSetupHardware(){
	
//Sets up state and frequency queues
	//Clock enabled to A & C
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	
	//LED setup to PA5
		GPIOA->MODER &= ~(3UL<<10);
		GPIOA->MODER |= 1UL<<10;
		
		GPIOA->OTYPER &= ~(1<<5);
		
		GPIOA->OSPEEDR &= ~(3UL<<10);
		
		GPIOA->PUPDR &= ~(3UL<<10);
	
	//Buttons setup
		GPIOC->MODER &= 0xF3FFFFFF;
	
	
	// TIM4
		//Enable clock
		RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
		
		//ControlRegistr1
			//Clear everything except the reserved bits
		TIM4->CR1 &= 0b1111010000000000;
			//Buffer ARR
		TIM4->CR1 |= 1UL<<7;
		
		// Skipping CR2, if problems, start here
		
		//DMA / Interrupt Control Register
			//Clear everything except reserved bits
		TIM4->DIER &= 0b1010000010100000;
			//Enable Update Interrupt
		TIM4->DIER |= 0x0001;
		
		//Clear Prescaler
		TIM4->PSC &= 0x0000;
		
		//Set ARR to 220 Hz
		TIM4->ARR &= 0x0000;
		//TIM4->ARR |= 0x008D; 440
		TIM4->ARR |= ARR_LUT[0];
		
		//Enable TIM4 counter
		//TIM4->CR1 |= 0x0001;
		
		// St priority of timer 4 interrupt to 1
		NVIC_SetPriority(TIM4_IRQn, 0);
		
		// Enable timer 4 interrupt controller
		NVIC_EnableIRQ(TIM4_IRQn);
		
		
	
	

	
	//DAC setup
		//Enable DAC clock
		RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
		//Disabe DAC
		DAC1->CR &= ~(DAC_CR_EN1 | DAC_CR_EN2); 
		//My Chemical Romance
		DAC1->MCR &= ~(7U<<16);
		//Trigger selection bits, dont need em
		DAC1->CR &= ~DAC_CR_TSEL1;
		//Select TIM4 as the trigger
		DAC1->CR |= (DAC_CR_TSEL1_0 | DAC_CR_TSEL1_2);
		//Enable DAC
		DAC1->CR |= DAC_CR_EN1;
		//Set PA3 to analog output mode (0b11)
		GPIOA->MODER |= 3U<<(2*3);

}


BaseType_t SetupQueue() {
	xStateQueue = xQueueCreate(1, sizeof(BaseType_t));
	//xFrequencyQueue = xQueueCreate(1, 2);
	
	if (xStateQueue != NULL){
		//uint8_t initial_state = 1;
		BaseType_t initial_state = 1;
		//char initial_freq = 1;
		
		xQueueOverwrite(xStateQueue, (void *) &initial_state);
		//xQueueOverwrite(xFrequencyQueue, &initial_freq);
		return 1;
	}
	else {
		return NULL; // In theory, will return NULL if either of the queues fails to create
	}
}


void TIM4_IRQHandler() {
	

	
		//increment a variable
		SLUT_CNT++;
		//if that var is too high, wrap to 0
		if (SLUT_CNT == 64){
			SLUT_CNT = 0;
		}
		
		//lookup SLUT, feed to DAC
		DAC->DHR12R1 = SLUT[SLUT_CNT];
	

	//Clear update interrupt flag (UIF)
	if ((TIM4->SR & TIM_SR_UIF) != 0) {
			TIM4->SR &= ~TIM_SR_UIF;
	}
	
	return;
	
}



void LEDtoggle(){
	
	while(1) {
		BaseType_t xLEDState = 0;
		xQueuePeek(xStateQueue, &xLEDState, 0);
		if(xLEDState == 1){
			//Turn on LED
			GPIOA->ODR |= 1UL<<5;
			//Enable TIM4 for the speaker
			TIM4->CR1 |= 0x0001;
		} else {
			//Turn off LED
			GPIOA->ODR &= ~1UL<<5;
			//Disable TIM4 for the speaker
			TIM4->CR1 &= ~0x0001;
			//Set DAC output to 0V
			DAC->DHR12R1 = 0x000;
		}
	}
}


void bouncyBoi() {

  // Create the state machine for debouncing
  enum debouncer_state_t {
    WAIT, PRESSED, RELEASED, TOGGLE
  };
  enum debouncer_state_t debouncer_state = WAIT;

  //Counter variable to keep track of time
  BaseType_t timer = 0;
	
	//Thing to check state of LED
	BaseType_t xLEDState = 0;
	
	//Next LED state
	BaseType_t new_state = 0;

  //Loop forever
  while(1) {
    //GRAB BUTTON STATE
    BaseType_t button = (GPIOC -> IDR) & 0x00002000;

    switch(debouncer_state){
    case WAIT:
      //Go to pressed state when pressed
      if (button == 0) {
        debouncer_state = PRESSED;
        timer = 0;
      } 
      break;
    
    case PRESSED:
      //Continues to be pressed, increment timer
      if (button == 0) {
        if (timer < 300) {
          timer += 1;
        }
      }
      // Button released, either go back to wait or go to released
      else {
        if (timer < 200) {
          debouncer_state = WAIT;
        } else {
          debouncer_state = RELEASED;
        }
        
        timer = 0;
      }
      break;
    
    case RELEASED:
      //if the button gets pressed again, return to pressed state
      if (button == 0) {
        debouncer_state = PRESSED;
        timer = 0;
      } 
      // button stays released for the required time
      else {
        if (timer > 10) {
          debouncer_state = TOGGLE;
          timer = 0;
        } else {
          timer += 1;
        }
      }
      break;
      
    case TOGGLE:
      //Toggle LED
			xQueuePeek(xStateQueue, &xLEDState, 0);
			if (xLEDState == 0) {
				new_state = 1;
			} else {
				new_state = 0;
			}
				
			//BaseType_t new_state = ~xLEDState;
      xQueueOverwrite(xStateQueue, &new_state);
				
				// Update ARR on every button press
//				ASS_CNT++;
//				if (ASS_CNT >= 8){
//					ASS_CNT = 0;
//				}
//				TIM4->ARR = ARR_LUT[ASS_CNT];
				
      //Return to waiting
      debouncer_state = WAIT;
      break;
    }
    
  }
  
}

void USART2_IRQHandler(){
	uint8_t buffer[sizeof(BaseType_t)] = {};
	BaseType_t pRx_counter = 0;
	
	if(USART2->ISR & USART_ISR_RXNE) { // Received data
		buffer[pRx_counter] = USART2->RDR;
		// Reading USART_DR automatically clears the RXNE flag
		(pRx_counter)++;
		if((pRx_counter) >= 1 )
			(pRx_counter) = 0;
	}
	
	BaseType_t frequency_index = buffer[pRx_counter] - 0x61;
	TIM4->ARR = ARR_LUT[frequency_index];
		//TIM4->ARR = ARR_LUT[0];
}

//void USART2_IRQHandler(void) {
//	USART_IRQHandler(sizeof(BaseType_t), &Rx2_Counter);

//	if (USART2_Buffer_Rx[Rx2_Counter - 1] == 0x74 || USART2_Buffer_Rx[Rx2_Counter - 1] == 0x54){
//		uint8_t buffer[] = {'t', 'e', 'm', 'p', ' ', 'u', 'n', 'a', 'v', 'a', 'i', 'l', 'a', 'b', 'l', 'e', '\n', 13};
//		USART_Write(USART2, buffer, 18);
//	}
//}