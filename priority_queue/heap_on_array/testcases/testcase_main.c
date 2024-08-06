#include <stdio.h>
#include <sys/time.h>

#include "heap.h"

int minComparator(const void* a, const void* b) {
	return *(uint64_t*)a - *(uint64_t*)b;
}

int maxComparator(const void* a, const void* b) {
	return -(*(uint64_t*)a - *(uint64_t*)b);
}

void testMinHeap() {
	struct timeval time;
	gettimeofday(&time, NULL);
	srand(time.tv_sec + time.tv_usec);

	heap_t* heap = NewMinHeap(256);

	uint64_t count = rand() % 131072;
	uint64_t* values = calloc(count, sizeof(uint64_t));
	for (int i = 0; i < count; i++) {
		uint64_t value = rand();
		values[i] = value;
		InsertIntoHeap(heap, value, (void*)value);
	}

	qsort(values, count, sizeof(uint64_t), minComparator);

	for (int i = 0; i < count; i++) {
		if ((uint64_t)DeleteFromHeap(heap) != values[i]) {
			perror("Not equal");
			exit(-1);
		}
	}
	printf("TEST(MinHeap) passed (values %lu)\n", count);

	FreeHeap(heap);
	free(values);
}

void testMaxHeap() {
	struct timeval time;
	gettimeofday(&time, NULL);
	srand(time.tv_sec + time.tv_usec);

	heap_t* heap = NewMaxHeap(256);

	uint64_t count = rand() % 131072;
	uint64_t* values = calloc(count, sizeof(uint64_t));
	for (int i = 0; i < count; i++) {
		uint64_t value = rand();
		values[i] = value;
		InsertIntoHeap(heap, value, (void*)value);
	}

	qsort(values, count, sizeof(uint64_t), maxComparator);

	for (int i = 0; i < count; i++) {
		if ((uint64_t)DeleteFromHeap(heap) != values[i]) {
			perror("Not equal");
			exit(-1);
		}
	}
	printf("TEST(MaxHeap) passed (values %lu)\n", count);

	FreeHeap(heap);
	free(values);
}

int main(void) {
	testMinHeap();
	testMaxHeap();
	return 0;
}
