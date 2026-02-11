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

void TIM4_IRQHandler();

