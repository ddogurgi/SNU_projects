/********************************************************
 * Filename: core/sync.c
 * 
 * Author: wsyoo, RTOSLab. SNU.
 * 
 * Description: semaphore, condition variable management.
 ********************************************************/
#include <core/eos.h>

void eos_init_semaphore(eos_semaphore_t *sem, int32u_t initial_count, int8u_t queue_type) {
	/* initialization */
	sem->count = initial_count;
	sem->queue_type = queue_type;
	sem->wait_queue = NULL;
}

int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout) {
	int32u_t flag = eos_disable_interrupt(); //disable interrupt
	if(sem->count > 0){//if the number of resource instance > 0
		sem->count--;//decrease instance by 1
		eos_restore_interrupt(flag); //enable interrupt
		return 1; //successful return
	}
	else{
		if(timeout == -1){
			eos_restore_interrupt(flag);
			return 0;
		}
		else{
			if(timeout > 0){
				_os_node_t* node =(_os_node_t*)malloc(sizeof(_os_node_t));
				node->priority = eos_get_current_task()->priority;
				node->ptr_data = eos_get_current_task();
				if(!sem->queue_type)
					_os_add_node_tail(&sem->wait_queue, node);
				else
					_os_add_node_priority(&sem->wait_queue, node);
				eos_restore_interrupt(flag);
				eos_sleep(timeout);
				int32u_t flag = eos_disable_interrupt();
				if(sem->count > 0){
					sem->count--;
					eos_restore_interrupt(flag);
					return 1;
				}
				else{
					eos_restore_interrupt(flag);
					return 0;
				}
			}
			else{
				while(1){
					_os_node_t* node =(_os_node_t*)malloc(sizeof(_os_node_t));
					node->priority = eos_get_current_task()->priority;
					node->ptr_data = eos_get_current_task();
					if(!sem->queue_type)
						_os_add_node_tail(&sem->wait_queue, node);
					else
						_os_add_node_priority(&sem->wait_queue, node);
					eos_restore_interrupt(flag);
					eos_suspend_task(eos_get_current_task());
					eos_schedule();
					int f = eos_disable_interrupt();
					if(sem->count > 0){
						sem->count--;
						eos_restore_interrupt(f);
						return 1;
					}
					free(node);
				}
			}
		}
	}
}

void eos_release_semaphore(eos_semaphore_t *sem) {
	int32u_t flag = eos_disable_interrupt();
	sem->count++;
	_os_node_t* head = sem->wait_queue;
	if(head){
		_os_remove_node(&sem->wait_queue, head);
		eos_tcb_t* task = (eos_tcb_t*)head->ptr_data;
		free(head);
		eos_restore_interrupt(flag);
		_os_wakeup_sleeping_task(task);
	}
	else
		eos_restore_interrupt(flag);
}

void eos_init_condition(eos_condition_t *cond, int32u_t queue_type) {
	/* initialization */
	cond->wait_queue = NULL;
	cond->queue_type = queue_type;
}

void eos_wait_condition(eos_condition_t *cond, eos_semaphore_t *mutex) {
	/* release acquired semaphore */
	eos_release_semaphore(mutex);
	/* wait on condition's wait_queue */
	_os_wait(&cond->wait_queue);
	/* acquire semaphore before return */
	eos_acquire_semaphore(mutex, 0);
}

void eos_notify_condition(eos_condition_t *cond) {
	/* select a task that is waiting on this wait_queue */
	_os_wakeup_single(&cond->wait_queue, cond->queue_type);
}
