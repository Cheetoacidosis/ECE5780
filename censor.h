#ifndef CENSOR_H
#define CENSOR_H

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "data.h"

void xUS100SensorRead();

void USART1_IRQHandler();

extern QueueHandle_t SensorQueue;

extern QueueHandle_t reading;

#endif 
