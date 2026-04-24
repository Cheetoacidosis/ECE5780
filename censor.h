#ifndef CENSOR_H
#define CENSOR_H

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "USART.h"

#define VOLDIST 30

void xUS100SensorRead();

extern QueueHandle_t SensorQueue;
extern QueueHandle_t frequency_queue;
extern QueueHandle_t volume_queue;
extern QueueHandle_t freq_peek_queue;
extern QueueHandle_t vol_peek_queue;

#endif 
