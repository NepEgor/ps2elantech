#include "queue.h"
/*
bool queue_empty(struct queue *q)
{
  return q.size == 0;
}

bool queue_full(struct queue *q)
{
  return q.size == QCAPACITY;
}
*/
Queue::Queue()
{
    head = 0;
    tail = 0;
}

uint8_t Queue::Size()
{
    return size;
}

// returns 1 if full
uint8_t Queue::push(queue_elem elem)
{
    if (size < QCAPACITY)
    {
        container[head++] = elem;
        size++;
        if (head >= QCAPACITY)
        {
            head = 0;
        }

        return 0;
    }

    return 1;
}

// returns 1 if empty
uint8_t Queue::pull(queue_elem &elem)
{
    if (size > 0)
    {
        elem = container[tail++];
        size--;
        if (tail >= QCAPACITY)
        {
            tail = 0;
        }

        return 0;
    }

    return 1;
}
