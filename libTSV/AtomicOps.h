#ifndef ATOMIC_OPS_H_INCLUDED
#define ATOMIC_OPS_H_INCLUDED

typedef long atomic32_t;
typedef long long atomic64_t;

// 32bits version
atomic32_t AtomicLoad(volatile atomic32_t* value);
atomic32_t AtomicExchange(volatile atomic32_t* value, atomic32_t newValue);
atomic32_t AtomicAdd(volatile atomic32_t* value, atomic32_t added);
atomic32_t AtomicSub(volatile atomic32_t* value, atomic32_t subed);
atomic32_t AtomicAddRetLast(volatile atomic32_t* value, atomic32_t added);
atomic32_t AtomicSubRetLast(volatile atomic32_t* value, atomic32_t subed);

// 64bits version
atomic64_t AtomicLoad(volatile atomic64_t* value);
atomic64_t AtomicExchange(volatile atomic64_t* value, atomic64_t newValue);
atomic64_t AtomicAdd(volatile atomic64_t* value, atomic64_t added);
atomic64_t AtomicSub(volatile atomic64_t* value, atomic64_t subed);
atomic64_t AtomicAddRetLast(volatile atomic64_t* value, atomic64_t added);
atomic64_t AtomicSubRetLast(volatile atomic64_t* value, atomic64_t subed);

#if defined(_MSC_VER)
    #include "platform_win\AtomicOps_impl.h"
#elif defined(__GNUC__)
    #include "platform_linux\AtomicOps_impl.h"
#else
    #error No atomic support on your platform!
#endif

#endif // ATOMIC_OP_H_INCLUDED