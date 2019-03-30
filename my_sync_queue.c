
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
    pthread_cond_init(&queue.job_cv1,NULL);
    pthread_cond_init(&queue.job_cv2,NULL);
    return queue;
}

bool is_empty_sq(struct my_sync_queue *queue){
    return queue->write==queue->read;
}


void add_sq(struct my_sync_queue *queue, int element){
    printf("Producer about to get the lock...");
    pthread_mutex_lock(&queue->job_mutex);
    printf("P got the lock\n");
    while(queue->size >= LENGTH){
        pthread_cond_wait(&queue->job_cv2,&queue->job_mutex);
    }
    printf("P in the queue");
    //locked
        queue->buff[queue->write] = element;
        queue->write=(queue->write+1)%LENGTH;
        queue->size++;
    //unlocked
    pthread_mutex_unlock(&queue->job_mutex);
    pthread_cond_signal(&queue->job_cv1);
    printf("P placed element\n");
}

int remove_sq(struct my_sync_queue *queue){
    printf("Consumer about to get the lock...");
    pthread_mutex_lock(&queue->job_mutex);
    printf("C got the lock\n");
    while(queue->size<=0){
        printf("C Q size %d\n",queue->size);
        pthread_cond_wait(&queue->job_cv1,&queue->job_mutex);
    }
    printf("C in the queue");
    //locked
        int element = queue->buff[queue->read];
        queue->read = (queue->read+1)%LENGTH;
        queue->size--;
    //not locked

    pthread_mutex_unlock(&queue->job_mutex);
    pthread_cond_signal(&queue->job_cv2);
    printf("C got the element\n");
    return element;
}