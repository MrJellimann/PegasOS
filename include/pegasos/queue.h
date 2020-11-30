#ifndef pegasos_queue_h
#define pegasos_queue_h

#include <circle/alloc.h>
#include <pegasos/node.h>

// TODO 
// make task class to encapsulate tasks in pegasos
// write functions
// 

class Queue
{
public:

// is the queue empty?
boolean empty(void);

// how many tasks are in queue?
int size (void);

// push task on
// the function should resort based on priority
bool push(/* task variable here */ );


/* task variable here */ pop(/* task variable here */ );


private: 
// sort queue function

void sort(void);
// checking for starved tasks function
void checkStarved(void);
// weighting ? todo
protected:
}

#endif