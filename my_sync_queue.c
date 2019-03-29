
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_sync_queue.h"

struct my_sync_queue create_sync_queue(){
    struct my_sync_queue queue;
    queue.read=0;
    queue.write=0;
    queue.size=0;
    return queue;
}

bool is_empty_sq(struct my_sync_queue *queue){
    return queue->write==queue->read;
}


void add_sq(struct my_sync_queue *queue, int element){
    if(queue->size+1>=LENGTH){
        printf("Queue OverFlow\n");
        exit(1);
    }
    //queue->buff[queue->write]=j;
    queue->write=(queue->write+1)%LENGTH;
    //char* type = type_string(queue->buff[queue->read].type);
    queue->size++;
}

int remove_sq(struct my_sync_queue *queue){
    int element = queue->buff[queue->read];
    queue->read = (queue->read+1)%LENGTH;
    queue->size--;
    return element;
}