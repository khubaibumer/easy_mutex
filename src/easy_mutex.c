/*
 * easy_mutex.c
 *
 *  Created on: Nov 20, 2019
 *      Author: khubaibumer
 *      Inspired By: Gnome Library
 *
 */

#include "easy_mutex-posix.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
							/*	Mutex	*/

void e_mutex_init(eMutex_t *mutex) {

	mutex->i[0] = 0;
}

void e_mutex_clear(eMutex_t *mutex) {

	if (mutex->i[0] != 0)
		abort();
}

static void __attribute__((noinline))
e_mutex_lock_slowpath(eMutex_t *mutex) {

	while (exchange_acquire (&mutex->i[0], 2) != 0) {
		syscall(__NR_futex, &mutex->i[0], (esize) FUTEX_WAIT_PRIVATE, (esize) 2,
		NULL);
	}
}

static void __attribute__((noinline))
e_mutex_unlock_slowpath(eMutex_t *mutex,
		euint prev) {

	if E_UNLIKELY (prev == 0) {
		fprintf (stderr, "Attempt to unlock mutex that was not locked\n");
		abort ();
	}

	syscall (__NR_futex, &mutex->i[0], (esize) FUTEX_WAKE_PRIVATE, (esize) 1, NULL);
}

void e_mutex_lock(eMutex_t *mutex) {

	/* 0 -> 1 and we're done.  Anything else, and we need to wait... */
	if E_UNLIKELY (e_atomic_int_add (&mutex->i[0], 1) != 0)
			e_mutex_lock_slowpath (mutex);
}

void e_mutex_unlock(eMutex_t *mutex) {

	euint prev;
	prev = exchange_release(&mutex->i[0], 0);

	/* 1-> 0 and we're done.  Anything else and we need to signal... */
	if E_UNLIKELY (prev != 1)
		e_mutex_unlock_slowpath (mutex, prev);
}

ebool e_mutex_trylock(eMutex_t *mutex) {

	euint zero = 0;
	/* We don't want to touch the value at all unless we can move it from
	 * exactly 0 to 1.
	 */
	return compare_exchange_acquire (&mutex->i[0], &zero, 1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
						/*	RW-Lock	*/

static void*
e_rw_lock_impl_new(void) {


	pthread_rwlock_t *rwlock;
	eint status;

	rwlock = malloc(sizeof(pthread_rwlock_t));
	if E_UNLIKELY (rwlock == NULL) {
			fprintf(stderr,"Error:%d->%s, Func:%s\n", errno, strerror(errno), "malloc");
			abort();
	}

	if E_UNLIKELY ((status = pthread_rwlock_init (rwlock, NULL)) != 0) {
			fprintf(stderr,"Error:%d->%s, Func:%s\n", status, strerror(errno), "pthread_rwlock_init");
			abort();
	}
	return rwlock;
}

void e_rw_lock_init(eRwLock_t *rw_lock) {

	rw_lock->p = e_rw_lock_impl_new ();
}

static void e_rw_lock_impl_free(pthread_rwlock_t *rwlock) {

	pthread_rwlock_destroy(rwlock);
	free(rwlock);
}

void e_rw_lock_clear(eRwLock_t *rw_lock) {

	e_rw_lock_impl_free (rw_lock->p);
}

static inline pthread_rwlock_t*
e_rw_lock_get_impl(eRwLock_t *lock) {
	pthread_rwlock_t *impl = e_atomic_pointer_get(&lock->p);

	if E_UNLIKELY (impl == NULL) {

		impl = e_rw_lock_impl_new ();
		if (!e_atomic_pointer_compare_and_exchange (&lock->p, NULL, impl))
			e_rw_lock_impl_free (impl);
		impl = lock->p;
	}
	return impl;
}

void e_rw_lock_writer_lock(eRwLock_t *rw_lock) {

	 int retval = pthread_rwlock_wrlock (e_rw_lock_get_impl (rw_lock));

	  if (retval != 0)
	    fprintf (stderr,"Error->%s, Func:%s\n", strerror (retval), "pthread_rwlock_wrlock");
}

ebool e_rw_lock_writer_trylock(eRwLock_t *rw_lock) {

	if (pthread_rwlock_trywrlock(e_rw_lock_get_impl(rw_lock)) != 0)
		return false;

	return true;
}

void e_rw_lock_writer_unlock(eRwLock_t *rw_lock) {

	pthread_rwlock_unlock (e_rw_lock_get_impl (rw_lock));
}

void e_rw_lock_reader_lock(eRwLock_t *rw_lock) {

	int retval = pthread_rwlock_rdlock(e_rw_lock_get_impl(rw_lock));

	if (retval != 0)
		fprintf(stderr,"Error->%s, Func:%s\n", strerror(retval), "pthread_rwlock_rdlock");
}

ebool e_rw_lock_reader_trylock(eRwLock_t *rw_lock) {

	if (pthread_rwlock_tryrdlock(e_rw_lock_get_impl(rw_lock)) != 0)
		return false;

	return true;
}

void e_rw_lock_reader_unlock(eRwLock_t *rw_lock) {

	pthread_rwlock_unlock (e_rw_lock_get_impl (rw_lock));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
						/*	Recursive-Mutex		*/

static pthread_mutex_t*
e_rec_mutex_impl_new(void) {

	pthread_mutexattr_t attr;
	pthread_mutex_t *mutex;

	mutex = malloc(sizeof(pthread_mutex_t));
	if E_UNLIKELY (mutex == NULL)
			fprintf(stderr,"Error:%d->%s, Func:%s\n", errno, strerror(errno), "malloc");

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(mutex, &attr);
	pthread_mutexattr_destroy(&attr);

	return mutex;
}

void e_rec_mutex_init(eRecMutex_t *rec_mutex) {

	rec_mutex->p = e_rec_mutex_impl_new ();
}

static void e_rec_mutex_impl_free(pthread_mutex_t *mutex) {

	pthread_mutex_destroy(mutex);
	free(mutex);
}

void e_rec_mutex_clear(eRecMutex_t *rec_mutex) {

	e_rec_mutex_impl_free (rec_mutex->p);
}

static inline pthread_mutex_t*
e_rec_mutex_get_impl(eRecMutex_t *rec_mutex) {

	pthread_mutex_t *impl = e_atomic_pointer_get(&rec_mutex->p);

	if E_UNLIKELY (impl == NULL)
	{
		impl = e_rec_mutex_impl_new ();
		if (!e_atomic_pointer_compare_and_exchange (&rec_mutex->p, NULL, impl))
			e_rec_mutex_impl_free (impl);
		impl = rec_mutex->p;
	}

	return impl;
}

void e_rec_mutex_lock(eRecMutex_t *rec_mutex) {

	pthread_mutex_lock (e_rec_mutex_get_impl (rec_mutex));
}

void e_rec_mutex_unlock(eRecMutex_t *rec_mutex) {

	pthread_mutex_unlock (rec_mutex->p);
}

ebool e_rec_mutex_trylock(eRecMutex_t *rec_mutex) {

	if (pthread_mutex_trylock(e_rec_mutex_get_impl(rec_mutex)) != 0)
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
						/*	ConditionVar	*/

void e_cond_init(eCond_t *cond) {

	cond->i[0] = 0;
}

void e_cond_clear(eCond_t *cond) {
	// EMPTY FUNC()
}

void e_cond_wait(eCond_t *cond, eMutex_t *mutex) {

	euint sampled = e_atomic_int_get(&cond->i[0]);

	e_mutex_unlock(mutex);
	syscall(__NR_futex, &cond->i[0], (esize) FUTEX_WAIT_PRIVATE,
			(esize) sampled, NULL);
	e_mutex_lock(mutex);
}

void e_cond_signal(eCond_t *cond) {

	e_atomic_int_inc(&cond->i[0]);

	syscall(__NR_futex, &cond->i[0], (esize) FUTEX_WAKE_PRIVATE, (esize) 1,
			NULL);
}

void e_cond_broadcast(eCond_t *cond) {

	e_atomic_int_inc(&cond->i[0]);

	syscall(__NR_futex, &cond->i[0], (esize) FUTEX_WAKE_PRIVATE,
			(esize) INT_MAX, NULL);
}

ebool e_cond_wait_until(eCond_t *cond, eMutex_t *mutex, eint64 end_time) {

	struct timespec now;
	struct timespec span;
	euint sampled;
	int res;

	if (end_time < 0)
		return false;

	clock_gettime(CLOCK_MONOTONIC, &now);
	span.tv_sec = (end_time / 1000000) - now.tv_sec;
	span.tv_nsec = ((end_time % 1000000) * 1000) - now.tv_nsec;
	if (span.tv_nsec < 0) {
		span.tv_nsec += 1000000000;
		span.tv_sec--;
	}

	if (span.tv_sec < 0)
		return false;

	sampled = cond->i[0];
	e_mutex_unlock(mutex);
	res = syscall(__NR_futex, &cond->i[0], (esize) FUTEX_WAIT_PRIVATE,
			(esize) sampled, &span);
	e_mutex_lock(mutex);

	return (res < 0 && errno == ETIMEDOUT) ? false : true;
}

