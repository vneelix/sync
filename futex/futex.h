#ifndef FUTEX_H
#define FUTEX_H

#include <linux/futex.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

int futex(
	uint32_t *uaddr, int futex_op, uint32_t val,
	const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3
);

#endif //FUTEX_H
