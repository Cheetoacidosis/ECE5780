#include "my_queues.h"

BaseType_t SetupQueue() {
	xStateQueue = xQueueCreate(1, sizeof(BaseType_t));
	SensorQueueueueueueueueueueueueueueue = xQueueCreate(1, sizeof(BaseType_t));
	
	if (xStateQueue != NULL && SensorQueueueueueueueueueueueueueueue != NULL){
		BaseType_t initial_state = 1;
		
		xQueueOverwrite(xStateQueue, (void *) &initial_state);
		return 1;
	}
	else {
		return NULL; // In theory, will return NULL if either of the queues fails to create
	}
}

