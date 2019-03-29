//
// Created by Adam Berck on 2019-01-30.
//

#ifndef PROJECT1_GIORGIO_S_DISCRETE_EVENT_SIMULATOR_MY_FIFO_QUEUE_H
#define PROJECT1_GIORGIO_S_DISCRETE_EVENT_SIMULATOR_MY_FIFO_QUEUE_H


#define LENGTH 100

struct my_sync_queue {
    int buff[LENGTH];
    int read;
    int write;
    int size;
};

bool is_empty_fq(struct my_sync_queue *queue);

struct my_sync_queue create_sync_queue();

void add_sq(struct my_sync_queue *queue, int element);


int remove_sq(struct my_sync_queue *queue);


#endif //PROJECT1_GIORGIO_S_DISCRETE_EVENT_SIMULATOR_MY_FIFO_QUEUE_H