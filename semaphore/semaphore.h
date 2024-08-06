#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdatomic.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

#include "futex.h"

typedef uint32_t semaphore_t;

void	Acquire(semaphore_t* s);
void	Release(semaphore_t* s);
bool	TryAcquire(semaphore_t* s);

#endif //SEMAPHORE_H
