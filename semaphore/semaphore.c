#include "semaphore.h"

void Acquire(semaphore_t* s) {
	uint32_t counter;
	for (; ;) {
		counter = atomic_load((atomic_uint_least32_t*)s);
		if (counter > 0 && atomic_compare_exchange_strong((atomic_uint_least32_t*)s, &counter, counter - 1)) {
			break;
		}

		if (counter == 0) {
			int rc = futex(s, FUTEX_WAIT, 0, NULL, NULL, 0);
			if (rc == -1 && errno != EAGAIN) {
				perror("Semaphore FUTEX_WAIT failure");
			}
		}
	}
}

void Release(semaphore_t* s) {
	atomic_fetch_add((atomic_uint_least32_t*)s, 1);
	if (futex(s, FUTEX_WAKE, 1, NULL, NULL, 0) == -1) {
		perror("Semaphore FUTEX_WAKE failure");
	}
}

bool TryAcquire(semaphore_t* s) {
	uint32_t counter;
	for (; ;) {
		counter = atomic_load((atomic_uint_least32_t*)s);
		if (counter > 0 && atomic_compare_exchange_strong((atomic_uint_least32_t*)s, &counter, counter - 1)) {
			return true;
		}

		if (counter == 0) {
			return false;
		}
	}
}
