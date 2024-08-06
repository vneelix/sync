#ifndef MUTEX_H
#define MUTEX_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdbool.h>

#include "futex.h"

enum State {
	RELEASED,
	ACQUIRED
};

typedef struct Mutex {
	pid_t		tid;
	uint32_t	state;
} mutex_t;

typedef struct Mutex mutex_t;

mutex_t *	NewMutex();
void		FreeMutex(mutex_t * m);
void		Lock(mutex_t * m);
void		Unlock(mutex_t * m);
bool		TryLock(mutex_t * m);

#endif //MUTEX_H
