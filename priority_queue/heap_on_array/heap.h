#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct HeapNode {
	uint64_t	priority;
	void*		item;
} heap_node_t;

typedef struct Heap {
	heap_node_t*	array;
	uint64_t		length;
	uint64_t		capacity;

	bool (*cmp)(uint64_t, uint64_t);
} heap_t;

heap_t*	NewMaxHeap(uint64_t capacity);
heap_t*	NewMinHeap(uint64_t capacity);
void	InsertIntoHeap(heap_t* heap, uint64_t priority, void* item);
void*	DeleteFromHeap(heap_t* heap);
void*	TopElement(heap_t* heap);
void	FreeHeap(heap_t* heap);

#endif //HEAP_H
