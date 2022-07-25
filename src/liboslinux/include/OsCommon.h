
//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
/// SPDX-License-Identifier: MPL-2.0
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \author WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef D_OsCommon_H
#define D_OsCommon_H

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <assert.h>
#include <stddef.h>
#include <inttypes.h>
#include <sys/cdefs.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#ifndef BEGIN
#define BEGIN do {
#endif
#ifndef END
#define END } while(0)
#endif


#if   (__GNUC__ == 4 && __GNUC_MINOR__ == 4 ) // <- this version is bugged as a cast to void doesn't prevent the warning about unused result..
#define OS_WARN_UNUSED_RESULT
#elif    __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define OS_WARN_UNUSED_RESULT      \
  __attribute_warn_unused_result__
#else
#define OS_WARN_UNUSED_RESULT
#endif /* __GNUC__ */

#ifdef __GNUC__
#  define OS_ARG_NONNULL(params) __nonnull(params)
#else
#  define OS_ARG_NONNULL(params)
#endif


#ifdef __GNUC__
#  define OS_PURE() __attribute__((pure))
#else
#  define OS_PURE()
#endif

#ifdef __GNUC__
#  define OS_PACKED_POST __attribute__((aligned(1)))
#  define OS_PACKED_PRE
#else
#  define OS_PACKED_POST
#  define OS_PACKED_PRE
#  error Your compiler is currently not supported.
#endif

#ifdef __GNUC__
#  define likely(x)       __builtin_expect((x),1)
#  define unlikely(x)     __builtin_expect((x),0)
#else
#  define likely(x)       (x)
#  define unlikely(x)     (x)
#endif


#ifndef NDEBUG
#  define OS_ASSERT_FAIL(name)\
      BEGIN \
      const bool name=false; \
      assert(name);\
      END
#else
#  define OS_ASSERT_FAIL(name)
#endif

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#ifndef OS_BUILD_BUG_ON_ZERO
# define OS_BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#endif

#ifndef OS_BUILD_BUG_ON_NULL
# define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))
#endif

#ifndef OS_SAME_TYPE
# ifdef __GNUC__
#  define OS_SAME_TYPE(a, b) __builtin_types_compatible_p(__typeof__(a), __typeof__(b))
# else
#  define OS_SAME_TYPE(a, b)
# endif
#endif

#ifndef OS_MUST_BE_ARRAY
/* &a[0] degrades to a pointer: a different type from an array */
#define OS_MUST_BE_ARRAY(a) OS_BUILD_BUG_ON_ZERO(OS_SAME_TYPE((a), &(a)[0]))
#endif


#ifndef OS_ARRAY_SIZE
# define OS_ARRAY_SIZE(arr) ((size_t)(sizeof(arr)/sizeof(arr[0]) + OS_MUST_BE_ARRAY(arr)))
#endif

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define UNUSED(x) ((void)x)
/**
 * Macros around gcc's atomic buildins.
 */
#ifdef __GNUC__
# define OS_ATOMIC_FETCH_AND_ADD(ptr, value) __sync_fetch_and_add(ptr, value)
#else
# define OS_ATOMIC_FETCH_AND_ADD(ptr, value)
# error ATOMIC_FETCH_AND_ADD not available for your compiler!
#endif

#ifdef __GNUC__
# define OS_ATOMIC_FETCH_AND_SUB(ptr, value) __sync_fetch_and_sub(ptr, value)
#else
# define OS_ATOMIC_FETCH_AND_SUB(ptr, value)
# error ATOMIC_FETCH_AND_SUB not available for your compiler!
#endif

#ifdef __GNUC__
# define OS_ATOMIC_CAS(ptr, oldvalue, newvalue) __sync_bool_compare_and_swap(ptr, oldvalue, newvalue)
#else
# error OS_ATOMIC_CAS not available for your compiler!
#endif




#define OS_ATOMIC_INC(ptr) OS_ATOMIC_FETCH_AND_ADD(ptr, 1)
#define OS_ATOMIC_DEC(ptr) OS_ATOMIC_FETCH_AND_SUB(ptr, 1)

#define OS_ATOMIC_FETCH(var) ((volatile __typeof__(var))var)

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint_least8_t  ul8;
typedef uint_least16_t ul16;
typedef uint_least32_t ul32;
typedef uint_least64_t ul64;

typedef int_least8_t  il8;
typedef int_least16_t il16;
typedef int_least32_t il32;
typedef int_least64_t il64;

typedef uint_fast8_t  uf8;
typedef uint_fast16_t uf16;
typedef uint_fast32_t uf32;
typedef uint_fast64_t uf64;

typedef int_fast8_t  if8;
typedef int_fast16_t if16;
typedef int_fast32_t if32;
typedef int_fast64_t if64;


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

static __inline__ i8 min_i8( i8 a, i8 b)
{
   return (a < b) ? a : b;
}

static __inline__ i16 min_i16( i16 a, i16 b)
{
   return (a < b) ? a : b;
}

static __inline__ i32 min_i32( i32 a, i32 b)
{
   return (a < b) ? a : b;
}

static __inline__ i64 min_i64( i64 a, i64 b)
{
   return (a < b) ? a : b;
}

static __inline__ u8 min_u8( u8 a, u8 b)
{
   return (a < b) ? a : b;
}

static __inline__ u16 min_u16( u16 a, u16 b)
{
   return (a < b) ? a : b;
}

static __inline__ u32 min_u32( u32 a, u32 b)
{
   return (a < b) ? a : b;
}

static __inline__ u64 min_u64( u64 a, u64 b)
{
   return (a < b) ? a : b;
}


static __inline__ i8 max_i8( i8 a, i8 b)
{
   return (a > b) ? a : b;
}

static __inline__ i16 max_i16( i16 a, i16 b)
{
   return (a > b) ? a : b;
}

static __inline__ i32 max_i32( i32 a, i32 b)
{
   return (a > b) ? a : b;
}

static __inline__ i64 max_i64( i64 a, i64 b)
{
   return (a > b) ? a : b;
}

static __inline__ u8 max_u8( u8 a, u8 b)
{
   return (a > b) ? a : b;
}

static __inline__ u16 max_u16( u16 a, u16 b)
{
   return (a > b) ? a : b;
}

static __inline__ u32 max_u32( u32 a, u32 b)
{
   return (a > b) ? a : b;
}

static __inline__ u64 max_u64( u64 a, u64 b)
{
   return (a > b) ? a : b;
}


#endif  // D_OsCommon_H
