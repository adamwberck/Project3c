
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_sync_queue.h"

struct my_sync_queue create_sync_queue(){
    struct my_sync_queue queue;
    queue.read=0;
    queue.write=0;
    queue.size=0;
    pthread_mutex_init(&queue.job_mutex,NULL);
    return queue;
}

bool is_empty_sq(struct my_sync_queue *queue){
    return queue->write==queue->read;
}


void add_sq(struct my_sync_queue *queue, int element){
    pthread_mutex_lock(&queue->job_mutex);
    while(queue->size >= LENGTH){
        pthread_cond_wait(&queue->job_cv2,&queue->job_mutex);
    }
    //locked
        queue->buff[queue->write] = element;
        queue->write=(queue->write+1)%LENGTH;
        queue->size++;
    //unlocked
    pthread_mutex_unlock(&queue->job_mutex);
    pthread_cond_signal(&queue->job_cv1);
}

int remove_sq(struct my_sync_queue *queue){
    pthread_mutex_lock(&queue->job_mutex);
    while(queue->size<=0){
        pthread_cond_wait(&queue->job_cv1,&queue->job_mutex);
    }
    //locked
        int element = queue->buff[queue->read];
        queue->read = (queue->read+1)%LENGTH;
        queue->size--;
    //not locked
    pthread_mutex_unlock(&queue->job_mutex);
    pthread_cond_signal(&queue->job_cv2);
    return element;
}