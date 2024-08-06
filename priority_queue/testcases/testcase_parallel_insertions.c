#include <threads.h>
#include <string.h>
#include <limits.h>

#include "queue.h"

int minCMP(const void* a, const void* b) {
	return *(uint64_t*)a - *(uint64_t*)b;
}

int maxCMP(const void* a, const void* b) {
	return -(*(uint64_t*)a - *(uint64_t*)b);
}

typedef struct WorkerArgument {
	priority_queue_t*	queue;
	uint64_t			testCases;
	// futex variable
	uint32_t*			barrier;
} worker_argument_t;

int insertWorker(void* arg) {
	worker_argument_t* workerArg = (worker_argument_t*)arg;

	futex(workerArg->barrier, FUTEX_WAIT, 1, NULL, NULL, 0);

	for (uint64_t i = 0; i < workerArg->testCases; i++) {
		uint64_t value = rand() % 131072;
		InsertIntoPriorityQeue(workerArg->queue, value, (void*)value);
	}

	free(arg);
	return 0;
}

void testParallelInsertions(uint64_t threadsCount, bool minHeap) {
	thrd_t* threads = calloc(threadsCount, sizeof(thrd_t));
	if (threads == NULL) {
		perror("Failed to allocate threads arr");
		exit(-1);
	}

	uint64_t testCases = 32;
	priority_queue_t* queue =
		minHeap ?
			NewMinPriorityQueue(testCases * threadsCount) :
				NewMaxPriorityQueue(testCases * threadsCount);
	if (queue == NULL) {
		perror("Failed to create pqueue");
		exit(-1);
	}

	uint32_t barrier = 1;

	for (uint64_t i = 0; i < threadsCount; i++) {
		worker_argument_t* arg =
			(worker_argument_t*)malloc(sizeof(worker_argument_t));
		if (arg == NULL) {
			perror("Failed to alloc arg");
			exit(-1);
		}
		*arg = (worker_argument_t){
			.queue = queue,
			.testCases = testCases,
			.barrier = &barrier
		};
		if (thrd_create(threads + i, insertWorker, (void*)arg) == -1) {
			perror("Failed to create thread");
			exit(-1);
		}
	}

	barrier = 0;
	futex(&barrier, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);

	for (uint64_t i = 0; i < threadsCount; i++) {
		thrd_join(threads[i], NULL);
	}
	free(threads);

	uint64_t* array = calloc(threadsCount * testCases, sizeof(uint64_t));
	if (array == NULL) {
		perror("Failed to alloc arr");
		exit(-1);
	}
	uint64_t idx = 0;
	while (!IsEmptyPQ(queue)) {
		if (idx >= threadsCount * testCases) {
			printf("TESTCASE FAILED: pqueue size not equal testcases\n");
			exit(-1);
		}
		uint64_t value = (uint64_t)DeleteFromPriorityQeue(queue);
		array[idx++] = value;
	}
	uint64_t* sortedArray = memcpy(
		malloc(sizeof(uint64_t) * threadsCount * testCases), array,
		sizeof(uint64_t) * threadsCount * testCases
	);
	qsort(sortedArray, sizeof(uint64_t), threadsCount * testCases, minHeap ? minCMP : maxCMP);
	for (idx = 0; idx < threadsCount * testCases; idx++) {
		if (array[idx] != sortedArray[idx]) {
			printf("TESTCASE FAILED: incorrect elements order\n");
			exit(-1);
		}
	}

	printf("TESTCASE SUCCESS: values inserted %lu\n", threadsCount * testCases);
	free(array);
	free(sortedArray);
	FreePriorityQueue(queue);
}
