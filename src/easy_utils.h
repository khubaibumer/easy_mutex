/*
 * easy_utils.h
 *
 *  Created on: Nov 20, 2019
 *      Author: khubaibumer
 */

#ifndef EASY_UTILS_H_
#define EASY_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>
#include <linux/futex.h>
#include <asm/unistd_64.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <limits.h>

/*	Internal Types for libeasy_threadpool	*/
#ifndef __EASY_TYPES__
#define __EASY_TYPES__

#define ENULL NULL

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define EUINT_TO_POINTER(u)	((eptr) (eulong) (u))

typedef void *eptr;
typedef const void *econstptr;
typedef bool ebool;

typedef int eint;
typedef char echar;
typedef long elong;
typedef short eshort;

typedef unsigned int euint;
typedef unsigned char euchar;
typedef unsigned long eulong;
typedef unsigned short eushort;

typedef int8_t eint8;
typedef int16_t eint16;
typedef int32_t eint32;
typedef int64_t eint64;

typedef uint8_t euint8;
typedef uint16_t euint16;
typedef uint32_t euint32;
typedef uint64_t euint64;

typedef signed long essize;
typedef unsigned long esize;

typedef euint32 eQuark;

typedef float efloat;
typedef double edouble;

typedef void (*eFunc)(eptr args);
typedef void (*efunc_f)(eptr data, eptr user_data);

typedef eint (*eCmprDataFunc)(econstptr a, econstptr b, eptr user_data);

typedef void (*eDestroyNotify)(eptr data);

typedef struct {
	efunc_f _func;
	eptr _usr_data;
	ebool _is_shared;
} easy_threadpool_t;

typedef struct {
	eQuark domain;
	eint code;
	echar *msg;
} easy_error_t;

#endif /*	__EASY_TYPES__	*/

#define _e_mem_new(_type, _num, func) \
        ((_type *) e_##func##_n ((_num), sizeof (_type)))

#define _G_RENEW(struct_type, mem, n_structs, func) \
        ((struct_type *) g_##func##_n (mem, (n_structs), sizeof (struct_type)))

#define e_mem_new(_type, _num)  _e_mem_new(_type, _num, malloc)

#endif /* EASY_UTILS_H_ */
