#ifndef INTRUSIVEREFPTR_H_INCLUDED
#define INTRUSIVEREFPTR_H_INCLUDED

#include "RefCounted.h"

template <class T>
class IntrusiveRefPtr {
public:
    IntrusiveRefPtr(RefCounted<T>* p)
        : ptr_(static_cast<T*>(p))
    {
        acquire();
    }

    template <class U>
    IntrusiveRefPtr(const IntrusiveRefPtr<U>& that)
        : ptr_(static_cast<T*>(that.ptr_))
    {
        acquire();
    }

    ~IntrusiveRefPtr()
    {
        release();
    }

    COMMON_POINTER_IMPL(T, ptr_)

    void reset()
    {
        release();
        ptr_ = nullptr;
    }

protected:

    template <class T> friend class RefPtr;
    template <class T> friend class WeakRef;

    void acquire()
    {
        if (ptr_)
        {
            ptr_->acquire();
        }
    }

    void release()
    {
        if (ptr_)
        {
            ptr_->release();
        }
    }

    T* ptr_;
};

#endif // INTRUSIVEREFPTR_H_INCLUDED