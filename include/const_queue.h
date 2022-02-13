#ifndef PS2ELANTECH_QUEUE_H
#define PS2ELANTECH_QUEUE_H

#include <stdint.h>

typedef uint8_t queue_elem;

class ConstQueue
{
    public:

    static const uint16_t QCAPACITY = 6000;

    private:

    queue_elem container[QCAPACITY];
    uint16_t head;
    uint16_t tail;
    uint16_t size;

    public:

    ConstQueue();
    uint8_t push(queue_elem elem);
    uint8_t pull(queue_elem &elem);

    uint16_t Size();
    void clear();

};

#endif
