//
// Created by Adam Berck on 2019-01-30.
//

#ifndef PROJECT3C_MY_INT_SYNC_Q_H
#define PROJECT3C_MY_INT_SYNC_Q_H

#include <stdbool.h>
#include <pthread.h>
#define LENGTH 20

struct my_int_sync_queue {
    int buff[LENGTH];
    int read;
    int write;
    int size;
    pthread_mutex_t intq_mutex;
    pthread_cond_t intq_empty,intq_full;
};


struct my_int_sync_queue create_int_sync_queue();

void add_isq(struct my_int_sync_queue *queue, int element);

int remove_isq(struct my_int_sync_queue *queue);


#endif //PROJECT1_GIORGIO_S_DISCRETE_EVENT_SIMULATOR_MY_FIFO_QUEUE_H