/*
 * easy_atomic.h
 *
 *  Created on: Nov 20, 2019
 *      Author: khubaibumer
 *      Inspired By: Gnome Library
 */

#ifndef EASY_ATOMIC_H_
#define EASY_ATOMIC_H_

#define E_GNUC_EXTENSION __extension__

#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define _E_BOOLEAN_EXPR(expr)                   \
 E_GNUC_EXTENSION ({                            \
   int _e_bool_var_;                         \
   if (expr)                                    \
      _e_bool_var_ = 1;                      \
   else                                         \
      _e_bool_var_ = 0;                      \
   _e_bool_var_;                             \
})
#define E_LIKELY(expr) (__builtin_expect (_E_BOOLEAN_EXPR((expr)), 1))
#define E_UNLIKELY(expr) (__builtin_expect (_E_BOOLEAN_EXPR((expr)), 0))
#else
#define E_LIKELY(expr) (expr)
#define E_UNLIKELY(expr) (expr)
#endif

#define E_GNUC_FORMAT( arg_idx )                \
  __attribute__((__format_arg__ (arg_idx)))
#define E_GNUC_NORETURN                         \
  __attribute__((__noreturn__))
#define E_GNUC_CONST                            \
  __attribute__((__const__))
#define E_GNUC_UNUSED                           \
  __attribute__((__unused__))
#define E_GNUC_NO_INSTRUMENT			\
  __attribute__((__no_instrument_function__))

#ifndef __GI_SCANNER__ /* The static assert macro really confuses the introspection parser */
#define E_PASTE_ARGS(identifier1,identifier2) identifier1 ## identifier2
#define E_PASTE(identifier1,identifier2)      E_PASTE_ARGS (identifier1, identifier2)
#ifdef __COUNTER__
#define E_STATIC_ASSERT(expr) typedef char E_PASTE (_GStaticAssertCompileTimeAssertion_, __COUNTER__)[(expr) ? 1 : -1] E_GNUC_UNUSED
#else
#define E_STATIC_ASSERT(expr) typedef char E_PASTE (_GStaticAssertCompileTimeAssertion_, __LINE__)[(expr) ? 1 : -1] E_GNUC_UNUSED
#endif
#define E_STATIC_ASSERT_EXPR(expr) ((void) sizeof (char[(expr) ? 1 : -1]))
#endif

#define exchange_acquire(ptr, new) \
  __atomic_exchange_4((ptr), (new), __ATOMIC_ACQUIRE)
#define compare_exchange_acquire(ptr, old, new) \
  __atomic_compare_exchange_4((ptr), (old), (new), 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)

#define exchange_release(ptr, new) \
  __atomic_exchange_4((ptr), (new), __ATOMIC_RELEASE)
#define store_release(ptr, new) \
  __atomic_store_4((ptr), (new), __ATOMIC_RELEASE)

#define e_atomic_int_inc(atomic) \
  (E_GNUC_EXTENSION ({                                                       \
    E_STATIC_ASSERT (sizeof *(atomic) == sizeof (eint));                     \
    (void) (0 ? *(atomic) ^ *(atomic) : 1);                                  \
    (void) __sync_fetch_and_add ((atomic), 1);                               \
  }))

#define e_atomic_int_get(atomic) \
  (E_GNUC_EXTENSION ({                                                       \
    E_STATIC_ASSERT (sizeof *(atomic) == sizeof (eint));                     \
    (void) (0 ? *(atomic) ^ *(atomic) : 1);                                  \
    (eint) __atomic_load_4 ((atomic), __ATOMIC_SEQ_CST);                     \
  }))

#define e_atomic_pointer_compare_and_exchange(atomic, oldval, newval) \
  (E_GNUC_EXTENSION ({                                                       \
    E_STATIC_ASSERT (sizeof *(atomic) == sizeof (eptr));                 	 \
    (void) (0 ? (eptr) *(atomic) : NULL);                                	 \
    (ebool) __sync_bool_compare_and_swap ((atomic), (oldval), (newval));  	 \
  }))

#define e_atomic_pointer_get(atomic) \
  (E_GNUC_EXTENSION ({                                                       \
    E_STATIC_ASSERT (sizeof *(atomic) == sizeof (eptr));	                 \
    (eptr) __atomic_load_8 ((atomic), __ATOMIC_SEQ_CST);                 	 \
  }))

#define e_atomic_int_add(atomic, val) \
  (E_GNUC_EXTENSION ({                                                       \
    E_STATIC_ASSERT (sizeof *(atomic) == sizeof (eint));                     \
    (void) (0 ? *(atomic) ^ (val) : 1);                                      \
    (eint) __sync_fetch_and_add ((atomic), (val));                           \
  }))

#define e_atomic_int_dec_and_test(atomic) \
  (E_GNUC_EXTENSION ({                                                       \
    E_STATIC_ASSERT (sizeof *(atomic) == sizeof (eint));                     \
    (void) (0 ? *(atomic) ^ *(atomic) : 1);                                  \
    __sync_fetch_and_sub ((atomic), 1) == 1;                                 \
  }))

#endif /* EASY_ATOMIC_H_ */
