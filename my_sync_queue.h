//
// Created by Adam Berck on 2019-01-30.
//

#ifndef PROJECT1_GIORGIO_S_DISCRETE_EVENT_SIMULATOR_MY_FIFO_QUEUE_H
#define PROJECT1_GIORGIO_S_DISCRETE_EVENT_SIMULATOR_MY_FIFO_QUEUE_H

#include <stdbool.h>
#include <pthread.h>
#define LENGTH 3

struct my_sync_queue {
    int buff[LENGTH];
    int read;
    int write;
    int size;
    pthread_mutex_t job_mutex;
    pthread_cond_t job_cv1,job_cv2;
};

bool is_empty_fq(struct my_sync_queue *queue);

struct my_sync_queue create_sync_queue();

void add_sq(struct my_sync_queue *queue, int element);


int remove_sq(struct my_sync_queue *queue);


#endif //PROJECT1_GIORGIO_S_DISCRETE_EVENT_SIMULATOR_MY_FIFO_QUEUE_H