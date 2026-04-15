#include "my_queues.h"

BaseType_t SetupQueue() {
	xStateQueue = xQueueCreate(1, sizeof(BaseType_t));
	SensorQueue = xQueueCreate(1, sizeof(BaseType_t));
	frequency_queue = xQueueCreate(1, sizeof(BaseType_t));
	volume_queue = xQueueCreate(1, sizeof(BaseType_t));
	freq_peek_queue = xQueueCreate(1, sizeof(uint16_t));
	vol_peek_queue = xQueueCreate(1, sizeof(uint16_t));
	
	if (xStateQueue != NULL && SensorQueue != NULL && frequency_queue != NULL && volume_queue != NULL && freq_peek_queue != NULL && vol_peek_queue != NULL){
		BaseType_t initial_state = 1;
		
		xQueueOverwrite(xStateQueue, (void *) &initial_state);
		return 1;
	}
	else {
		return NULL; // In theory, will return NULL if either of the queues fails to create
	}
}

