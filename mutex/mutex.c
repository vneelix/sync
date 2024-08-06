#include "mutex.h"

static pid_t gettid() {
	pid_t tid = syscall(SYS_gettid);
	if (tid == -1) {
		perror("Failed to get thread id");
	}
	return tid;
}

mutex_t* NewMutex() {
	void* ptr = calloc(1, sizeof(mutex_t));
	if (ptr == NULL) {
		return NULL;
	}
	return ptr;
}

void FreeMutex(mutex_t* m) {
	free(m);
}

void Lock(mutex_t* m) {
	if (TryLock(m)) {
		return;
	}

	uint32_t state;
	for (; ;) {
		state = RELEASED;
		if (atomic_compare_exchange_strong((atomic_uint_least32_t*)&m->state, &state, ACQUIRED)) {
			m->tid = gettid();
			break;
		}

		const int rc = futex(&m->state, FUTEX_WAIT, ACQUIRED, NULL, NULL, 0);
		if (rc == -1 && errno != EAGAIN) {
			perror("Mutex FUTEX_WAIT failed");
			break;
		}
	}
}

void Unlock(mutex_t* m) {
	if (m->tid != gettid()) {
		return;
	}

	m->tid = 0;
	atomic_exchange((atomic_uint_least32_t*)&m->state, RELEASED);
	if (futex(&m->state, FUTEX_WAKE, 1, NULL, NULL, 0) == -1) {
		perror("Mutex FUTEX_WAKE failed");
	}
}

bool TryLock(mutex_t* m) {
	uint32_t state = RELEASED;
	if (atomic_compare_exchange_strong((atomic_uint_least32_t*)&m->state, &state, ACQUIRED)) {
		m->tid = gettid();
		return true;
	}

	if (m->tid == gettid()) {
		return true;
	}

	return false;
}
