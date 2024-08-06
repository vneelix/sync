#ifndef QUEUE_H
#define QUEUE_H

#include "heap.h"
#include "mutex.h"
#include "semaphore.h"

typedef struct PriorityQueue {
	heap_t*	heap;

	mutex_t*		insertMutex;
	mutex_t*		deleteMutex;
	semaphore_t		switcher;

	// futex variables
	uint32_t	isEmpty;
	uint32_t	isFull;
} priority_queue_t;

priority_queue_t*	NewMinPriorityQueue(uint64_t capacity);
priority_queue_t*	NewMaxPriorityQueue(uint64_t capacity);
void				FreePriorityQueue(priority_queue_t* q);
void				InsertIntoPriorityQeue(priority_queue_t* queue, uint64_t priority, void* item);
void*				DeleteFromPriorityQeue(priority_queue_t* queue);
void*				TopElementPQ(priority_queue_t* queue);
bool				IsEmptyPQ(priority_queue_t* queue);

#endif //QUEUE_H
