#include "queue.h"
/*
bool queue_empty(struct queue_t *q)
{
  return q.size == 0;
}

bool queue_full(struct queue_t *q)
{
  return q.size == QCAPACITY;
}
*/
Queue::Queue()
{
    head = QCAPACITY - 1;
    tail = QCAPACITY - 1;
}

// return 1 if full
uint8_t Queue::push(queue_elem_t elem)
{
    if (size < QCAPACITY)
    {
        container[head--] = elem;
        size++;
        if (head < 0)
        {
        head = QCAPACITY-1;
        }

        return 0;
    }

    return 1;
}

// returns 1 if empty
uint8_t Queue::pull(queue_elem_t &elem)
{
    if (size > 0)
    {
        elem = container[tail--];
        size--;
        if (tail < 0)
        {
        tail = QCAPACITY-1;
        }

        return 0;
    }

    return 1;
}
