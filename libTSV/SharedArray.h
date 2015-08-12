#ifndef SHAREDARRAY_H_INCLUDED
#define SHAREDARRAY_H_INCLUDED

#include "SharedBuf.h"

template <typename T>
class SharedArray {
public:
    typedef typename SharedBuf<T>::Bytes Bytes;

    SharedArray()
        : buf_(reinterpret_cast<Bytes*>(Null.data))
    {
        acquire();
    }

    SharedArray(size_t size)
        : buf_(SharedBuf<T>::construct(nullptr, size))
    {
        if (!buf_) {
            buf_ = reinterpret_cast<Bytes*>(Null.data);
            acquire();
        }
    }

    SharedArray(const T* p, size_t size)
        : buf_(SharedBuf<T>::construct(p, size))
    {
        if (!buf_) {
            buf_ = reinterpret_cast<Bytes*>(Null.data);
            acquire();
        }
    }

    SharedArray(Bytes* buf)
    {
        if (buf != buf_) {
            buf_ = buf;
            acquire();
        }
    }

    SharedArray(const SharedArray& that)
        : buf_(that.buf_)
    {
        acquire();
    }

    ~SharedArray()
    {
        release();
    }

    SharedArray& operator=(const SharedArray& that)
    {
        if (this != &that) {
            release();
            buf_ = that.buf_;
            acquire();
        }
        return *this;
    }

    inline size_t capacity() const
    {
        return SharedBuf<T>::size(buf_);
    }

    inline const T* readonly() const
    {
        return static_cast<const T*>(
            static_cast<void*>(buf_));
    }

    inline T* writable()
    {
        if (SharedBuf<T>::refs(buf_) > 1) {
            Bytes* newBuf = SharedBuf<T>::clone(buf_);
            release();
            buf_ = newBuf;
        }

        return static_cast<T*>(
            static_cast<void*>(buf_));
    }

    inline const T& operator[](size_t i) const
    {
        const T* array = readonly();
        return array[i];
    }

protected:

    friend class Any;

    inline void acquire() const
    {
        SharedBuf<T>::acquire(buf_);
    }

    inline void release() const
    {
        SharedBuf<T>::release(buf_);
    }

    Bytes* buf_;
};

typedef SharedArray<uint8_t> ByteArray;
typedef SharedArray<char>    StringArray;

#endif // SHAREDARRAY_H_INCLUDED