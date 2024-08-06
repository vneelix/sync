#include "futex.h"

int futex(
	uint32_t *uaddr, int futex_op, uint32_t val,
	const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3
) {
	return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}
