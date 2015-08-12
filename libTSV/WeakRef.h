#ifndef WEAKREF_H_INCLUDED
#define WEAKREF_H_INCLUDED

#include "IntrusiveRefPtr.h"
#include "RefPtr.h"

template <class T>
class WeakRef {
public:
    template <class U>
    WeakRef(const RefPtr<U>& that)
        : refCount_(that.refCount_)
        , ptr_(that.ptr_)
    {
        acquire();
    }

    template <class U>
    WeakRef(const IntrusiveRefPtr<U>& that)
        : refCount_(static_cast<RefCounted<U>*>(that.ptr_)->refCount_)
        , ptr_(static_cast<T*>(that.ptr_))
    {
        acquire();
    }

    ~WeakRef()
    {
        release();
    }

    RefPtr<T> lock()
    {
        if (!hardRefs())
        {
            return RefPtr<T>();
        }
        return RefPtr<T>(ptr_, refCount_);
    }

protected:

    int32_t acquire()
    {
        return refCount_->weakAcquire();
    }

    int32_t release()
    {
        return refCount_->weakRelease();
    }

    int32_t hardRefs()
    {
        return refCount_->hardRefs();
    }

    T* ptr_;
    RefCountedBase* refCount_;
};

#endif // WEAKREF_H_INCLUDED