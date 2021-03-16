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
    head = QCAPACITY - 1;
    tail = QCAPACITY - 1;
}

// return 1 if full
uint8 Queue::push(queue_elem elem)
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
uint8 Queue::pull(queue_elem &elem)
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
