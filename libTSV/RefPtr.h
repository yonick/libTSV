#ifndef REFPTR_H_INCLUDED
#define REFPTR_H_INCLUDED

#include "RefCounted.h"

template <class T>
class RefPtr {
public:
    RefPtr()
        : ptr_(nullptr)
        , refCount_(nullptr)
    {
    }

    RefPtr(T* p)
        : ptr_(p)
        , refCount_(RefCountedImpl<T>::create(p))
    {
        acquire();
    }

    template <class U>
    RefPtr(const IntrusiveRefPtr<U>& that)
        : refCount_(static_cast<RefCounted<U>*>(that.ptr_)->refCount_)
        , ptr_(static_cast<T*>(that.ptr_))
    {
        acquire();
    }

    ~RefPtr()
    {
        release();
    }

    void reset(T* p = nullptr)
    {
        release();
        ptr_ = p;
        refCount_ = p ? RefCountedImpl<T>::create(p) : nullptr;
        acquire();
    }

    COMMON_POINTER_IMPL(T, ptr_)

protected:

    template <class T> friend class WeakRef;

    RefPtr(T* p, RefCountedBase* ref)
        : ptr_(p)
        , refCount_(ref)
    {
    }

    void acquire()
    {
        if (refCount_)
        {
            refCount_->hardAcquire();
        }
    }

    void release()
    {
        if (refCount_)
        {
            refCount_->hardRelease();
        }
    }

    T* ptr_;
    RefCountedBase* refCount_;
};

#endif // REFPTR_H_INCLUDED