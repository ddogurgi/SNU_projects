/********************************************************
 * Filename: core/comm.c
 *  
 * Author: jtlim, RTOSLab. SNU.
 * 
 * Description: message queue management. 
 ********************************************************/
#include <core/eos.h>
#include <string.h>

void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type) {
    mq->queue_start = queue_start;
    mq->queue_size = queue_size;
    mq->msg_size = msg_size;
    mq->queue_type = queue_type;
    mq->front = 0;
    mq->rear = 0;
    eos_init_semaphore(&mq->putsem, queue_size, queue_type);
    eos_init_semaphore(&mq->getsem, 0, queue_type);
}

int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout) {
    //acquire putsem
    if(!eos_acquire_semaphore(&mq->putsem, timeout))
        return 0;
    //putting message to the queue
    char* rear = (char*)mq->queue_start + mq->rear*mq->msg_size;
    memcpy(rear, message, mq->msg_size);
    if(mq->rear < mq->queue_size - 1)
        mq->rear++;
    else
        mq->rear=0;
    //release getsem
    eos_release_semaphore(&mq->getsem);
    return 1;
}

int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout) {
    //acquire getsem
    if(!eos_acquire_semaphore(&mq->getsem, timeout))
        return 0;
    //getting message from the queue
    char* front = (char*)mq->queue_start + mq->front*mq->msg_size;
    memcpy(message, front, mq->msg_size);
    if(mq->front < mq->queue_size - 1)
        mq->front++;
    else
        mq->front=0;
    //release putsem
    eos_release_semaphore(&mq->putsem);
    return 1;
}
