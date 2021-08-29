#ifndef QUEUE_H
#define QUEUE_H

#include <Arduino.h>

typedef uint8_t queue_elem;

class Queue
{
    public:

    static const uint16_t QCAPACITY = 6000;

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
    void clear();

};

#endif
