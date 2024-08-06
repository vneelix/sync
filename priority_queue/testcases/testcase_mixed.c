#include <threads.h>
#include <string.h>
#include <limits.h>

#include "queue.h"

typedef struct WorkerArgument {
	priority_queue_t*	queue;
	uint64_t			testCases;
	// futex variable
	uint32_t*			barrier;
} worker_argument_t;

int insertWorkerP(void* arg) {
	worker_argument_t* workerArg = (worker_argument_t*)arg;

	futex(workerArg->barrier, FUTEX_WAIT, 1, NULL, NULL, 0);

	for (uint64_t i = 0; i < workerArg->testCases; i++) {
		uint64_t value = rand() % 131072;
		InsertIntoPriorityQeue(workerArg->queue, value, (void*)value);
	}

	free(arg);
	return 0;
}

int deleteWorker(void* arg) {
	worker_argument_t* workerArg = (worker_argument_t*)arg;

	futex(workerArg->barrier, FUTEX_WAIT, 1, NULL, NULL, 0);

	for (uint64_t i = 0; i < workerArg->testCases / 2; i++) {
		DeleteFromPriorityQeue(workerArg->queue);
	}

	free(arg);
	return 0;
}

void testMixed() {
	uint64_t threadsCount = 24;
	thrd_t* threads = calloc(threadsCount + 2, sizeof(thrd_t));
	if (threads == NULL) {
		perror("Failed to allocate threads arr");
		exit(-1);
	}

	uint64_t testCases = 32;
	priority_queue_t* queue = NewMaxPriorityQueue(8);
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
		if (thrd_create(threads + i, insertWorkerP, (void*)arg) == -1) {
			perror("Failed to create thread");
			exit(-1);
		}
	}

	// create readers
	worker_argument_t* arg =
			(worker_argument_t*)malloc(sizeof(worker_argument_t));
	if (arg == NULL) {
		perror("Failed to alloc arg");
		exit(-1);
	}
	*arg = (worker_argument_t){
		.queue = queue,
		.testCases = testCases * threadsCount,
		.barrier = &barrier
	};
	if (thrd_create(threads + threadsCount, deleteWorker, (void*)arg) == -1) {
		perror("Failed to create reader worker_1 ");
		exit(-1);
	}
	arg = (worker_argument_t*)malloc(sizeof(worker_argument_t));
	if (arg == NULL) {
		perror("Failed to alloc arg");
		exit(-1);
	}
	*arg = (worker_argument_t){
		.queue = queue,
		.testCases = testCases * threadsCount,
		.barrier = &barrier
	};
	if (thrd_create(threads + threadsCount + 1, deleteWorker, (void*)arg) == -1) {
		perror("Failed to create reader worker_2 ");
		exit(-1);
	}

	barrier = 0;
	futex(&barrier, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);

	for (uint64_t i = 0; i < threadsCount + 2; i++) {
		thrd_join(threads[i], NULL);
	}
	free(threads);

	printf("TESTCASE SUCCESS\n");
	FreePriorityQueue(queue);
}
