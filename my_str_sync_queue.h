//
// Created by Adam Berck on 2019-01-30.
//

#ifndef PROJECT3C_MY_STR_SYNC_Q_H
#define PROJECT3C_MY_STR_SYNC_Q_H

#include <stdbool.h>
#include <pthread.h>
#define LENGTH 20

struct my_str_sync_queue{
    char* buff[LENGTH];
    int read;
    int write;
    int size;
    pthread_mutex_t str_mutex;
    pthread_cond_t str_empty,str_full;
};


struct my_str_sync_queue create_str_sync_queue();

void add_ssq(struct my_str_sync_queue *queue, char* element);


char* remove_ssq(struct my_str_sync_queue *queue);


#endif //PROJECT1_GIORGIO_S_DISCRETE_EVENT_SIMULATOR_MY_FIFO_QUEUE_H