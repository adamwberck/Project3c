
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_int_sync_queue.h"

struct my_int_sync_queue create_int_sync_queue(){
    struct my_int_sync_queue queue;
    queue.read=0;
    queue.write=0;
    queue.size=0;
    //init pthread thread things
    pthread_mutex_init(&queue.intq_mutex,NULL);
    pthread_cond_init(&queue.intq_empty,NULL);
    pthread_cond_init(&queue.intq_full,NULL);
    return queue;
}
//add to int queue
void add_isq(struct my_int_sync_queue *queue, int element){
    pthread_mutex_lock(&queue->intq_mutex);//get the lock
    while(queue->size >= LENGTH){//wait if queue is full
        pthread_cond_wait(&queue->intq_full,&queue->intq_mutex);
    }
    //locked
        queue->buff[queue->write] = element;
        queue->write=(queue->write+1)%LENGTH;
        queue->size++;
    //unlocked
    pthread_mutex_unlock(&queue->intq_mutex);//unlock
    pthread_cond_signal(&queue->intq_empty);//signal queue is no longer empty
}

//remove from int queue
int remove_isq(struct my_int_sync_queue *queue){
    pthread_mutex_lock(&queue->intq_mutex);//get the lock
    while(queue->size<=0){//wait if queue is empty
        pthread_cond_wait(&queue->intq_empty,&queue->intq_mutex);
    }
    //locked
        int element = queue->buff[queue->read];
        queue->read = (queue->read+1)%LENGTH;
        queue->size--;
    //not locked
    pthread_mutex_unlock(&queue->intq_mutex);//unlock
    pthread_cond_signal(&queue->intq_full);//signal queue is not full
    return element;
}