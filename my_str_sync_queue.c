
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_str_sync_queue.h"

struct my_str_sync_queue create_str_sync_queue(){
    struct my_str_sync_queue queue;
    queue.read=0;
    queue.write=0;
    queue.size=0;
    //pthread init everything
    pthread_mutex_init(&queue.str_mutex,NULL);
    pthread_cond_init(&queue.str_empty,NULL);
    pthread_cond_init(&queue.str_full,NULL);
    return queue;
}

//add to str queue
void add_ssq(struct my_str_sync_queue *queue, char* element){
    pthread_mutex_lock(&queue->str_mutex);//get the lock
    while(queue->size >= LENGTH){//wait if the queue is full
        pthread_cond_wait(&queue->str_full,&queue->str_mutex);
    }
    //locked
        queue->buff[queue->write] = element;
        queue->write=(queue->write+1)%LENGTH;
        queue->size++;
    //unlocked
    pthread_mutex_unlock(&queue->str_mutex);//unlock
    pthread_cond_signal(&queue->str_empty);//signal the queue is not empty
}
//remove from str queue
char* remove_ssq(struct my_str_sync_queue *queue){
    pthread_mutex_lock(&queue->str_mutex);//get the lock
    while(queue->size<=0){//wait if the queue is empty
        pthread_cond_wait(&queue->str_empty,&queue->str_mutex);
    }
    //locked
        char* element = queue->buff[queue->read];
        queue->read = (queue->read+1)%LENGTH;
        queue->size--;
    //unlocked
    pthread_mutex_unlock(&queue->str_mutex);//unlock
    pthread_cond_signal(&queue->str_full);//signal the queue is not full
    return element;
}