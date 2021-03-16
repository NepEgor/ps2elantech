#ifndef QUEUE_H
#define QUEUE_H

#define QCAPACITY 8

#include <Arduino.h>

typedef uint8 queue_elem;

class Queue
{
    private:

    queue_elem container[QCAPACITY];
    uint8 head;
    uint8 tail;
    uint8 size;

    public:

    Queue();
    uint8 push(queue_elem elem);
    uint8 pull(queue_elem &elem);

};

#endif
