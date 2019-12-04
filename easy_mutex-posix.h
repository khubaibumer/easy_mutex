/*
 * easy_mutex-posix.h
 *
 *  Created on: Nov 21, 2019
 *      Author: khubaibumer
 */

#ifndef EASY_MUTEX_POSIX_H_
#define EASY_MUTEX_POSIX_H_

#include "easy_mutex.h"
#include "easy_atomic.h"

			/*	Mutex	*/
void            e_mutex_init                    (eMutex_t *mutex);

void            e_mutex_clear                   (eMutex_t *mutex);

void            e_mutex_lock                    (eMutex_t *mutex);

ebool           e_mutex_trylock                 (eMutex_t *mutex);

void            e_mutex_unlock                  (eMutex_t*mutex);

			/*	RW-Lock	*/
void            e_rw_lock_init                  (eRwLock_t *rw_lock);

void            e_rw_lock_clear                 (eRwLock_t *rw_lock);

void            e_rw_lock_writer_lock           (eRwLock_t *rw_lock);

ebool           e_rw_lock_writer_trylock        (eRwLock_t *rw_lock);

void            e_rw_lock_writer_unlock         (eRwLock_t *rw_lock);

void            e_rw_lock_reader_lock           (eRwLock_t *rw_lock);

ebool           e_rw_lock_reader_trylock        (eRwLock_t *rw_lock);

void            e_rw_lock_reader_unlock         (eRwLock_t *rw_lock);

			/*	Recursive-Mutex		*/
void            e_rec_mutex_init                (eRecMutex_t *rec_mutex);

void            e_rec_mutex_clear               (eRecMutex_t *rec_mutex);

void            e_rec_mutex_lock                (eRecMutex_t *rec_mutex);

ebool           e_rec_mutex_trylock             (eRecMutex_t *rec_mutex);

void            e_rec_mutex_unlock              (eRecMutex_t *rec_mutex);

			/*	ConditionVar	*/
void            e_cond_init                     (eCond_t *cond);

void            e_cond_clear                    (eCond_t *cond);

void            e_cond_wait                     (eCond_t *cond, eMutex_t *mutex);

void            e_cond_signal                   (eCond_t *cond);

void            e_cond_broadcast                (eCond_t *cond);

ebool           e_cond_wait_until               (eCond_t *cond, eMutex_t *mutex, eint64 end_time);


#endif /* EASY_MUTEX_POSIX_H_ */
