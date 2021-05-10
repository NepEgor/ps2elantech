#ifndef QUEUE_H
#define QUEUE_H

#define QCAPACITY 8

#include <Arduino.h>

typedef uint8_t queue_elem;

class Queue
{
    private:

    queue_elem container[QCAPACITY];
    uint8_t head;
    uint8_t tail;
    uint8_t size;

    public:

    Queue();
    uint8_t push(queue_elem elem);
    uint8_t pull(queue_elem &elem);

};

#endif
