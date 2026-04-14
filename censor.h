#ifndef CENSOR_H
#define CENSOR_H

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "USART.h"

void xUS100SensorRead();

extern QueueHandle_t SensorQueue;
extern QueueHandle_t reading;
extern QueueHandle_t reading_USART3;

#endif 
