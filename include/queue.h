#ifndef QUEUE_H
#define QUEUE_H

#include <Arduino.h>

typedef uint8_t queue_elem;

class Queue
{
    public:

    static const uint8_t QCAPACITY = 12;

    private:

    queue_elem container[QCAPACITY];
    uint8_t head;
    uint8_t tail;
    uint8_t size;

    public:

    Queue();
    uint8_t push(queue_elem elem);
    uint8_t pull(queue_elem &elem);

    uint8_t Size();

};

#endif
