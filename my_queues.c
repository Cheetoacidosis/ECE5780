#include "my_queues.h"

BaseType_t SetupQueue() {
	xStateQueue = xQueueCreate(1, sizeof(BaseType_t));
	SensorQueue = xQueueCreate(1, sizeof(BaseType_t));
	frequency_queue = xQueueCreate(1, sizeof(BaseType_t));
	volume_queue = xQueueCreate(1, sizeof(BaseType_t));
	freq_peek_queue = xQueueCreate(1, sizeof(uint16_t));
	vol_peek_queue = xQueueCreate(1, sizeof(uint16_t));
	
	//Initialize vol & freq peek queues
//	uint16_t init_val = 1;
//	xQueueOverwrite(vol_peek_queue, &init_val);
//	xQueueOverwrite(freq_peek_queue, &init_val);
	
	if (xStateQueue != NULL && SensorQueue != NULL && frequency_queue != NULL && volume_queue != NULL && freq_peek_queue != NULL && vol_peek_queue != NULL){
		BaseType_t initial_state = 1;
		
		xQueueOverwrite(xStateQueue, (void *) &initial_state);
		return 1;
	}
	else {
		return NULL; // In theory, will return NULL if either of the queues fails to create
	}
}


//Before anything else, put non-garbage data into the queues
void InitializeQueues(){
	while(1){
		
	//Initialize vol & freq peek queues
//	uint16_t init_val = 1;
//	xQueueOverwrite(vol_peek_queue, &init_val);
//	xQueueOverwrite(freq_peek_queue, &init_val);
		
		
		// Passing NULL will cause the calling task to be deleted.
		vTaskDelete(NULL);
	}
}