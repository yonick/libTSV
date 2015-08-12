#ifndef INTRUSIVE_ATOMIC_OPS_IMPL_H_INCLUDED
#define INTRUSIVE_ATOMIC_OPS_IMPL_H_INCLUDED

#include <intrin.h>

#ifdef _M_X64
  #pragma intrinsic(_InterlockedExchangeAdd64)
  #pragma intrinsic(_InterlockedExchange64)
#else
  #pragma intrinsic(_InterlockedExchangeAdd)
  #pragma intrinsic(_InterlockedExchange)
#endif
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_ReadWriteBarrier)

inline bool
AtomicCompareAndSwap(volatile atomic64_t* value, atomic64_t o, atomic64_t n)
{
    return _InterlockedCompareExchange64(value, n, o) == o;
}

inline bool
AtomicCompareAndSwap(volatile atomic32_t* value, atomic32_t o, atomic32_t n)
{
    return _InterlockedCompareExchange(value, n, o) == o;
}

inline atomic64_t
AtomicExchange(volatile atomic64_t* value, atomic64_t newValue)
{
}

inline atomic32_t
AtomicExchange(volatile atomic32_t* value, atomic32_t newValue)
{
}

inline atomic64_t
AtomicLoad(volatile atomic64_t* value)
{
    atomic64_t ret = *value;
    _ReadWriteBarrier();
    return ret;
}

inline atomic32_t
AtomicLoad(volatile atomic32_t* value)
{
    atomic32_t ret = *value;
    _ReadWriteBarrier();
    return ret;
}

inline atomic64_t
AtomicAdd(volatile atomic64_t* value, atomic64_t added)
{
#ifdef _M_X64
    return _InterlockedExchangeAdd64(value, added) + added;
#else
    atomic64_t o, n;
    do {
        o = *value;
        n = o + added;
    } while (!AtomicCompareAndSwap(value, o, n));
    return n;
#endif
}

inline atomic32_t
AtomicAdd(volatile atomic32_t* value, atomic32_t added)
{
    return _InterlockedExchangeAdd(value, added) + added;
}

inline atomic64_t
AtomicSub(volatile atomic64_t* value, atomic64_t subed)
{
#ifdef _M_X64
    return _InterlockedExchangeAdd64(value, -subed) - subed;
#else
    atomic64_t o, n;
    do {
        o = *value;
        n = o - subed;
    } while (!AtomicCompareAndSwap(value, o, n));
    return n;
#endif
}

inline atomic32_t
AtomicSub(volatile atomic32_t* value, atomic32_t subed)
{
    return _InterlockedExchangeAdd(value, -subed) - subed;
}

inline atomic64_t
AtomicAddRetLast(volatile atomic64_t* value, atomic64_t added)
{
}

inline atomic64_t
AtomicSubRetLast(volatile atomic64_t* value, atomic64_t subed)
{
}

#endif // INTRUSIVE_ATOMIC_OPS_IMPL_H_INCLUDED