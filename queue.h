#ifndef QUEUE_H
#define QUEUE_H

#define QCAPACITY 8

#include <Arduino.h>

typedef uint8_t queue_elem_t;

class Queue
{
    private:

    queue_elem_t container[QCAPACITY];
    uint8_t head;
    uint8_t tail;
    uint8_t size;

    public:

    Queue();
    uint8_t push(queue_elem_t elem);
    uint8_t pull(queue_elem_t &elem);

};

#endif
