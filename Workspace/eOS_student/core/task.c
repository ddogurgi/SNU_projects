/********************************************************
 * Filename: core/task.c
 * 
 * Author: parkjy, RTOSLab. SNU.
 * 
 * Description: task management.
 ********************************************************/
#include <core/eos.h>

#define READY		1
#define RUNNING		2
#define WAITING		3

/*
 * Queue (list) of tasks that are ready to run.
 */
static _os_node_t *_os_ready_queue[LOWEST_PRIORITY + 1];

/*
 * Pointer to TCB of running task
 */
static eos_tcb_t *_os_current_task;

int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority) {
	PRINT("task: 0x%x, priority: %d\n", (int32u_t)task, priority);
	task->sp = _os_create_context(sblock_start, sblock_size, entry, arg); //create context of task
	task->priority = priority; //update tcb of task
	task->state = READY;//update tcb of task
	_os_node_t* node = (_os_node_t*)malloc(sizeof(_os_node_t)); //create the node for ready queue
	node->priority = priority;
	node->ptr_data = task;
	_os_set_ready(priority);//set priority bit 1
	_os_add_node_tail(_os_ready_queue + priority, node); //add the node in the ready queue
	return 0;
}

int32u_t eos_destroy_task(eos_tcb_t *task) {
}

void eos_schedule() {
	if (_os_current_task != NULL) { //if current task is present
		addr_t stp = _os_save_context(); //save context, return stack pointer	
		if (stp != NULL)
			_os_current_task -> sp = stp; //save stack pointer of running task
		else
			return; //exit function
		//state transition of current task
		if(_os_current_task->state == RUNNING){
			_os_current_task->state = READY;
			int32u_t _priority = _os_current_task->priority;
			_os_node_t* node = (_os_node_t*)malloc(sizeof(_os_node_t)); //create the node for pushing current task to ready queue
			node->priority = _priority;
			node->ptr_data = _os_current_task;
			_os_set_ready(_priority);//set priority bit 1
			_os_add_node_tail(_os_ready_queue + _priority, node); //push 
		}
		_os_current_task = NULL;
	}
	_os_node_t *next_node;
	int32u_t priority = _os_get_highest_priority();//get highest priority of ready queue
	if(priority > LOWEST_PRIORITY)
		return;
	next_node = _os_ready_queue[priority];
	_os_remove_node(_os_ready_queue + priority, next_node);//remove the node from ready queue
	if(_os_ready_queue[priority] == NULL) {//after removing, if there is no task in queue, set priority bit 0
		_os_unset_ready(priority); 
	}
	_os_current_task = next_node->ptr_data; //current task <= next task
	free(next_node);
	_os_current_task->state = RUNNING; //state transition of current task
	_os_restore_context(_os_current_task->sp); //restore context of next task	
}

eos_tcb_t *eos_get_current_task() {
	return _os_current_task;
}

void eos_change_priority(eos_tcb_t *task, int32u_t priority) {
}

int32u_t eos_get_priority(eos_tcb_t *task) {
}

void eos_set_period(eos_tcb_t *task, int32u_t period){
	task->period = period;
}

int32u_t eos_get_period(eos_tcb_t *task) {
}

int32u_t eos_suspend_task(eos_tcb_t *task) {
	task->state = WAITING;
	return 0;
}

int32u_t eos_resume_task(eos_tcb_t *task) {
}

void eos_sleep(int32u_t tick) {
	_os_current_task->state = WAITING; //Delayed
	if(tick > 0 || _os_current_task->period > 0){
		int32u_t timeout = (tick) ? tick : _os_current_task->period; //if tick > 0 : wait that amount of time
		//tick = 0 : wait the time of period.
		int32u_t priority = (LOWEST_PRIORITY+1)* timeout + _os_current_task->priority;//priority for alarm queue
		eos_counter_t *counter = eos_get_system_timer(); //one global system timer
		if(_os_current_task->alarm == NULL){
			eos_alarm_t *alarm = (eos_alarm_t*)malloc(sizeof(eos_alarm_t)); //initialize the alarm
			_os_current_task->alarm = alarm; //set alarm of current task
		}
		eos_set_alarm(counter, _os_current_task->alarm, priority, _os_wakeup_sleeping_task, _os_current_task); //set alarm
	}
	eos_schedule(); //scheduling
}

void _os_init_task() {
	PRINT("initializing task module.\n");

	/* init current_task */
	_os_current_task = NULL;

	/* init multi-level ready_queue */
	int32u_t i;
	for (i = 0; i < LOWEST_PRIORITY; i++) {
		_os_ready_queue[i] = NULL;
	}
}

void _os_wait(_os_node_t **wait_queue) {
}

void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type) {
}

void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type) {
}

void _os_wakeup_sleeping_task(void *arg) {
	eos_tcb_t* task = (eos_tcb_t*)arg; //arg is void pointer, so make it eos_tcb_t struct
	task->state = READY; //set the state READY
	_os_node_t* node =(_os_node_t*)malloc(sizeof(_os_node_t));
	node->priority = task->priority;
	node->ptr_data = task;
	_os_set_ready(task->priority); //set bit 1 of bitmap
	_os_add_node_tail(_os_ready_queue + task->priority, node); //push to ready queue
	eos_schedule(); //scheduling
}
