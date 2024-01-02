/********************************************************
 * Filename: core/timer.c
 *
 * Author: wsyoo, RTOSLab. SNU.
 * 
 * Description: 
 ********************************************************/
#include <core/eos.h>

static eos_counter_t system_timer;

int8u_t eos_init_counter(eos_counter_t *counter, int32u_t init_value) {
	counter->tick = init_value;
	counter->alarm_queue = NULL;
	return 0;
}

void eos_set_alarm(eos_counter_t* counter, eos_alarm_t* alarm, int32u_t timeout, void (*entry)(void *arg), void *arg) {
	//1. remove alarm from alarm queue of counter
	_os_remove_node(&(counter->alarm_queue), &(alarm->alarm_queue_node));
	//2. Return when timeout is 0 or entry is NULL
	if(timeout == 0 || entry == NULL){
		return;
	}
	//3. Set the field of alarm
	alarm->timeout = timeout /(LOWEST_PRIORITY+1);
	alarm->handler = entry;
	alarm->arg = arg;
	alarm->alarm_queue_node.ptr_data = alarm;
	alarm->alarm_queue_node.priority = timeout;
	//4. add the alarm to alarm queue
	_os_add_node_priority(&(counter->alarm_queue), &(alarm->alarm_queue_node));
}

eos_counter_t* eos_get_system_timer() {
	return &system_timer;
}

void eos_trigger_counter(eos_counter_t* counter) {
	PRINT("tick\n");
	//1. increment counter tick
	counter->tick++;
	_os_node_t* alarm_queue = counter->alarm_queue;
	eos_alarm_t* alarm = alarm_queue->ptr_data;
	while(counter->alarm_queue != NULL){
		alarm->timeout--;
		alarm_queue->priority = alarm_queue->priority - (LOWEST_PRIORITY + 1);
		alarm_queue = alarm_queue->next;
		alarm = alarm_queue->ptr_data;
		if(alarm_queue == counter->alarm_queue)
			break;
	}
	//2. call the call-back function
	while(counter->alarm_queue != NULL){
		alarm = counter->alarm_queue->ptr_data;
		if(alarm->timeout <= 0){			
			eos_set_alarm(counter, alarm, 0, NULL, NULL);
			alarm->handler(alarm->arg);
		}
		else return;
	}
}

/* Timer interrupt handler */
static void timer_interrupt_handler(int8s_t irqnum, void *arg) {
	/* trigger alarms */
	eos_trigger_counter(&system_timer);
}

void _os_init_timer() {
	eos_init_counter(&system_timer, 0);

	/* register timer interrupt handler */
	eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0, timer_interrupt_handler, NULL);
}
