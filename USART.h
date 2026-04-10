#ifndef CENSOR_H
#define CENSOR_H

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "data.h"

void USART1_IRQHandler();

void USART3_IRQHandler();

extern QueueHandle_t SensorQueue;

extern QueueHandle_t reading;

extern QueueHandle_t reading_USART3;

#endif 
