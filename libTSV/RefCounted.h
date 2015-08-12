#ifndef REFCOUNTED_H_INCLUDED
#define REFCOUNTED_H_INCLUDED

#include "AtomicOps.h"
#include "Utilities.h"
#include <stdint.h>
#include <assert.h>

#define COMMON_POINTER_IMPL(T, p) \
    T& operator*() const { return *p; } \
    T* operator->() const { return p; } \
    T* get() const { return p; } \
    operator bool() const { return !!p; }

namespace detail {

union RCPair {
    struct {
        int32_t h;
        int32_t w;
    } pair;
    atomic64_t a;
};

}

static const detail::RCPair kHardRCAtom = { 1, 1 };
static const detail::RCPair kWeakRCAtom = { 0, 1 };
static const detail::RCPair kInitialRC  = { 0, 0 };

class RefCountedBase {
public:
    RefCountedBase()
        : refCnt_(kInitialRC)
    {
    }

    virtual void destroyObj() const {}
    virtual void destroyRef() const {}

    int32_t hardRefs() const
    {
        detail::RCPair refs;
        refs.a = AtomicLoad(&refCnt_.a);
        return refs.pair.h;
    }

    int32_t hardAcquire() const
    {
        AtomicAdd(&refCnt_.a, kHardRCAtom.a);
        return refCnt_.pair.h;
    }

    int32_t weakAcquire() const
    {
        AtomicAdd(&refCnt_.a, kWeakRCAtom.a);
        return refCnt_.pair.h;
    }

    int32_t hardRelease() const
    {
        AtomicSub(&refCnt_.a, kHardRCAtom.a);
        assert(refCnt_.pair.h >= 0 && refCnt_.pair.w >= 0);
        int32_t hard = refCnt_.pair.h;
        if (!hard) {
            destroyObj();
        }
        if (!refCnt_.pair.w) {
            destroyRef();
        }
        return hard;
    }

    int32_t weakRelease() const
    {
        AtomicSub(&refCnt_.a, kWeakRCAtom.a);
        assert(refCnt_.pair.h >= 0 && refCnt_.pair.w >= 0);
        if (!refCnt_.pair.w) {
            destroyRef();
        }
        return refCnt_.pair.h;
    }

protected:
    mutable detail::RCPair refCnt_;
};

template <class T>
class RefCountedImpl : public RefCountedBase {
public:
    static RefCountedImpl<T>* create(T* p)
    {
        return new RefCountedImpl<T>(p);
    }

protected:
    virtual void destroyObj() const
    {
        delete static_cast<const T*>(object_);
    }

    virtual void destroyRef() const
    {
        delete this;
    }

private:
    RefCountedImpl(T* p)
        : object_(p)
    {
    }

    ~RefCountedImpl()
    {
        std::cout << "dtor of RefCountedImpl " << std::endl;
    }

    const T* object_;
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355)
#endif

template <class T>
class RefCounted : private noCopy {
public:
    RefCounted()
        : refCount_(
            RefCountedImpl<T>::create(
                static_cast<T*>(this)))
    {
        std::cout << "RefCounted<T> " << refCount_ << std::endl;
    }

    int32_t acquire() const
    {
        return refCount_->hardAcquire();
    }

    int32_t release() const
    {
        return refCount_->hardRelease();
    }

protected:

    template <class T> friend class RefPtr;
    template <class T> friend class WeakRef;

    RefCountedBase* refCount_;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // REFCOUNTED_H_INCLUDED