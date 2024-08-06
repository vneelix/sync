#include "queue.h"

static priority_queue_t* newPriorityQueue(uint64_t capacity, bool minHeap) {
	priority_queue_t* queue = (priority_queue_t*)calloc(1, sizeof(priority_queue_t));
	if (queue == NULL) {
		return NULL;
	}
	*queue = (priority_queue_t){
		.heap = minHeap ? NewMinHeap(capacity) : NewMaxHeap(capacity),

		.insertMutex = NewMutex(),
		.deleteMutex = NewMutex(),
		.switcher = (semaphore_t)(1),
		.isEmpty = 1,
		.isFull = 0
	};
	if (queue->heap == NULL || queue->insertMutex == NULL || queue->deleteMutex == NULL) {
		FreePriorityQueue(queue);
		return NULL;
	}
	return queue;
}

priority_queue_t* NewMinPriorityQueue(uint64_t capacity) {
	return newPriorityQueue(capacity, true);
}

priority_queue_t* NewMaxPriorityQueue(uint64_t capacity) {
	return newPriorityQueue(capacity, false);
}

void FreePriorityQueue(priority_queue_t* q) {
	if (q == NULL) {
		return;
	}
	if (q->heap != NULL) {
		FreeHeap(q->heap);
	}
	if (q->insertMutex != NULL) {
		FreeMutex(q->insertMutex);
	}
	if (q->deleteMutex != NULL) {
		FreeMutex(q->deleteMutex);
	}
	free(q);
}

void InsertIntoPriorityQeue(priority_queue_t* queue, uint64_t priority, void* item) {
	// queuing parallel inserts
	Lock(queue->insertMutex);

	// check available capacity
	uint32_t isFull = atomic_load((atomic_uint_least32_t*)&queue->isFull);
	if (isFull) {
		int rc = futex(&queue->isFull, FUTEX_WAIT, 1, NULL, NULL, 0);
		if (rc == -1 && errno != EAGAIN) {
			perror("PQueue FUTEX_WAIT failure (isFull)");
		}
	}

	// queuing two parallalel possible operations - insert/delete
	Acquire(&queue->switcher);
	// insert new item
	InsertIntoHeap(queue->heap, priority, item);
	// each inserting resets empty flag
	queue->isEmpty = 0;
	if (queue->heap->length == queue->heap->capacity) {
		queue->isFull = 1;
	}
	Release(&queue->switcher);

	// notify thread with delete operation
	int rc = futex(&queue->isEmpty, FUTEX_WAKE, 1, NULL, NULL, 0);
	if (rc == -1) {
		perror("PQueue FUTEX_WAKE failure (insert)");
	}

	Unlock(queue->insertMutex);
}

void*	DeleteFromPriorityQeue(priority_queue_t* queue) {
	// queuing parallel deletions
	Lock(queue->deleteMutex);

	// check available items
	uint32_t isEmpty = atomic_load((atomic_uint_least32_t*)&queue->isEmpty);
	if (isEmpty) {
		int rc = futex(&queue->isEmpty, FUTEX_WAIT, 1, NULL, NULL, 0);
		if (rc == -1 && errno != EAGAIN) {
			perror("PQueue FUTEX_WAIT failure (isEmpty)");
		}
	}

	// queuing two parallalel possible operations - insert/delete
	Acquire(&queue->switcher);
	// remove item
	void* item = DeleteFromHeap(queue->heap);
	// each remove resets full flag
	queue->isFull = 0;
	if (queue->heap->length == 0) {
		queue->isEmpty = 1;
	}
	Release(&queue->switcher);

	// notify thread with insert operation
	int rc = futex(&queue->isFull, FUTEX_WAKE, 1, NULL, NULL, 0);
	if (rc == -1) {
		perror("PQueue FUTEX_WAKE failure (delete)");
	}

	Unlock(queue->deleteMutex);
	return item;
}

void* TopElementPQ(priority_queue_t* queue) {
	return TopElement(queue->heap);
}

bool IsEmptyPQ(priority_queue_t* queue) {
	return queue->heap->length == 0;
}