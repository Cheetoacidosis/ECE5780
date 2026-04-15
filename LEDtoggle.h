#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


// Setting up queues
BaseType_t SetupQueue();

// GPIO for LED & Button, TIM4 setup
void prvSetupHardware();

// Reads LEDstate, turns LED on if 1
void LEDtoggle();

// Reads the button, and debounces it
void bouncyBoi();
void change_frequency();
void change_volume();

void TIM4_IRQHandler();

extern QueueHandle_t xStateQueue;
extern QueueHandle_t frequency_queue;
extern QueueHandle_t volume_queue;
extern QueueHandle_t freq_peek_queue;
extern QueueHandle_t vol_peek_queue;
