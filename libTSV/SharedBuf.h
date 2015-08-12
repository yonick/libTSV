#ifndef SHAREDBUF_H_INCLUDED
#define SHAREDBUF_H_INCLUDED

#include "AtomicOps.h"
#include "TypeTraits.h"
#include <assert.h>

struct Void {};

template <typename T>
struct SharedBuf {

    struct Bytes : Void {
        T bytes[16];
    };

    static Bytes* create(size_t size)
    {
        Bytes* result = nullptr;
        if (size) {
            const size_t allocSize =
                sizeof(SharedBuf) + (size - 1) * sizeof(T);

            SharedBuf* buf = static_cast<SharedBuf*>(malloc(allocSize));
            buf->refCount = 1;
            buf->capacity = size;
            buf->data = result =
                static_cast<Bytes*>(
                    static_cast<void*>(&buf->storage));
        }
        return result;
    }

    static Bytes* construct(const T* p, size_t size)
    {
        Bytes* ret = create(size);
        if (!ret) {
            return ret;
        }

        T* dst = static_cast<T*>(static_cast<void*>(ret));
        if (!p) {
            for (; size > 0; size--) {
                new(dst++) T();
            }
        } else {
            if (IsPod<T>::value) {
                memcpy(dst, p, size * sizeof(T));
            } else {
                for (const T* src = p; size > 0; size--) {
                    new(dst++) T(*src++);
                }
            }
        }
        return ret;
    }

    static inline SharedBuf* fromBytes(Bytes* p)
    {
        return static_cast<SharedBuf*>(
            static_cast<void*>(
                static_cast<unsigned char*>(static_cast<void*>(p))
                    - offsetof(SharedBuf, storage)));
    }

    static inline size_t size(Bytes* p)
    {
        return fromBytes(p)->size();
    }

    static inline Bytes* clone(Bytes* p)
    {
        return construct(
            static_cast<T*>(
                static_cast<void*>(p)), size(p));
    }

    static inline atomic32_t acquire(Bytes* p)
    {
        return fromBytes(p)->acquire();
    }

    static inline atomic32_t release(Bytes* p)
    {
        SharedBuf* buf = fromBytes(p);
        atomic32_t refs = buf->release();
        assert(refs >= 0);
        if (!refs) {
            if (!IsPod<T>::value) {
                T* p = static_cast<T*>(
                    static_cast<void*>(buf->data));
                for (size_t size = buf->size(); size > 0; size--) {
                    (p++)->~T();
                }
            }
            free(buf);
        }
        return refs;
    }

    static inline atomic32_t refs(Bytes* p)
    {
        return fromBytes(p)->refs();
    }

    inline size_t size()
    {
        return capacity;
    }

    inline atomic32_t acquire()
    {
        return AtomicAdd(&refCount, 1);
    }

    inline atomic32_t release()
    {
        return AtomicSub(&refCount, 1);
    }

    inline atomic32_t refs()
    {
        return AtomicLoad(&refCount);
    }

    atomic32_t refCount;
    size_t     capacity;
    Bytes*     data;
    T          storage[1];
};

extern SharedBuf<char> Null;

#endif // SHAREDBUF_H_INCLUDED