#include "heap.h"

static bool maxHeap(uint64_t l, uint64_t r) {
	return l >= r;
}

static bool minHeap(uint64_t l, uint64_t r) {
	return l <= r;
}

static heap_t* newHeap(uint64_t capacity, bool (*cmp)(uint64_t, uint64_t)) {
	heap_t* heap = (heap_t*)malloc(sizeof(heap_t));
	if (heap == NULL) {
		return NULL;
	}

	*heap = (heap_t){
		.array = (heap_node_t*)calloc(capacity, sizeof(heap_node_t)),
		.length = 0,
		.capacity = capacity,
		.cmp = cmp
	};
	if (heap->array == NULL) {
		free(heap);
		return NULL;
	}

	return heap;
}

heap_t* NewMaxHeap(uint64_t capacity) {
	return newHeap(capacity, maxHeap);
}

heap_t* NewMinHeap(uint64_t capacity) {
	return newHeap(capacity, minHeap);
}

void FreeHeap(heap_t* heap) {
	if (heap == NULL) {
		return;
	}
	if (heap->array != NULL) {
		free(heap->array);
	}
	free(heap);
}

static void balanceUp(heap_t* heap) {
	uint64_t idx = heap->length;
	while (idx) {
		uint64_t parentIdx = (idx - (idx % 2 == 0 ? 2 : 1)) / 2;

		if (heap->cmp(heap->array[parentIdx].priority, heap->array[idx].priority)) {
			break;
		}

		heap_node_t node = heap->array[parentIdx];
		heap->array[parentIdx] = heap->array[idx];
		heap->array[idx] = node;

		idx = parentIdx;
	}
}

void InsertIntoHeap(heap_t* heap, uint64_t priority, void* item) {
	if (heap->length == heap->capacity) {
		heap->capacity += heap->capacity;
		heap->array = realloc(heap->array, sizeof(heap_node_t) * heap->capacity);
		if (heap->array == NULL) {
			return;
		}
	}

	heap->array[heap->length] = (heap_node_t){
		.priority = priority,
		.item = item
	};
	balanceUp(heap);
	heap->length++;
}

static heap_node_t* getLeftChild(heap_t* heap, uint64_t idx) {
	uint64_t childIdx = 2 * idx + 1;
	if (childIdx > heap->length) {
		return NULL;
	}
	return heap->array + childIdx;
}

static heap_node_t* getRightChild(heap_t* heap, uint64_t idx) {
	uint64_t childIdx = 2 * idx + 2;
	if (childIdx > heap->length) {
		return NULL;
	}
	return heap->array + childIdx;
}

static void balanceDown(heap_t* heap) {
	uint64_t idx = 0;
	while (idx < heap->length) {

		uint64_t childIdx = 0;
		heap_node_t* child = NULL;
		heap_node_t* leftChild = getLeftChild(heap, idx);
		heap_node_t* rightChild = getRightChild(heap, idx);

		if (leftChild && rightChild) {
			child = leftChild;
			childIdx = 2  * idx + 1;
			if (heap->cmp(rightChild->priority, leftChild->priority)) {
				child = rightChild;
				childIdx = 2  * idx + 2;
			}
		}
		else if (leftChild) {
			child = leftChild;
			childIdx = 2  * idx + 1;
		} else if (rightChild) {
			child = rightChild;
			childIdx = 2  * idx + 2;
		} else {
			break;
		}

		if (heap->cmp(heap->array[idx].priority, child->priority)) {
			break;
		}
		heap_node_t node = heap->array[childIdx];
		heap->array[childIdx] = heap->array[idx];
		heap->array[idx] = node;

		idx = childIdx;
	}
}

void* DeleteFromHeap(heap_t* heap) {
	if (heap->length == 0) {
		return NULL;
	}
	void* item = heap->array[0].item;

	heap->length--;
	heap->array[0] = heap->array[heap->length];
	balanceDown(heap);

	return item;
}

void* TopElement(heap_t* heap) {
	if (heap->length == 0) {
		return NULL;
	}
	return heap->array[0].item;
}
