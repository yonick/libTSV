#ifndef ANY_H_INCLUDED
#define ANY_H_INCLUDED

#include "RefCounted.h"
#include "SharedArray.h"
#include <initializer_list>
#include <stdint.h>

class Any {
public:

    enum Type {
        eNULL,
        eINT8,
        eINT16,
        eINT32,
        eINT64,
        eBOOL,
        eDOUBLE,
        eREFOBJ,
        eARRAY,
    };

    template <typename T> struct TypeInfo {
        typedef struct {} Type;
    };

    Any() : type_(eNULL) {}

    ~Any() { release(); }

    Any(int8_t  i) : type_(eINT8  ) { u_.int8Val   = i; }
    Any(int16_t i) : type_(eINT16 ) { u_.int16Val  = i; }
    Any(int32_t i) : type_(eINT32 ) { u_.int32Val  = i; }
    Any(int64_t i) : type_(eINT64 ) { u_.int64Val  = i; }
    Any(bool    b) : type_(eBOOL  ) { u_.boolVal   = b; }
    Any(double  d) : type_(eDOUBLE) { u_.doubleVal = d; }

    template <typename T>
    Any(const IntrusiveRefPtr<T>& ref)
        : type_(eREFOBJ)
    {
        u_.refObj = RefObj::from(ref);
        u_.refObj->acquire();
    }

    Any(std::initializer_list<Any> il)
        : type_(eARRAY)
    {
        u_.array = static_cast<Void*>(
            SharedBuf<Any>::construct(il.begin(), il.size()));
    }

    Any(const SharedArray<Any>& array)
        : type_(eARRAY)
    {
        array.acquire();
        u_.array = static_cast<Void*>(array.buf_);
    }

    Any(const Any& that)
        : type_(that.type_)
        , u_(that.u_)
    {
        acquire();
    }

    Any& operator=(const Any& that)
    {
        if (this != &that) {
            release();
            type_ = that.type_;
            u_ = that.u_;
            acquire();
        }
        return *this;
    }

protected:

    template <typename T> friend struct AnyCastHelper;

    inline void acquire()
    {
        if (type_ == eREFOBJ) {
            u_.refObj->acquire();
        } else if (type_ == eARRAY) {
            SharedBuf<Any>::acquire(
                static_cast<SharedBuf<Any>::Bytes*>(u_.array));
        }
    }

    inline void release()
    {
        if (type_ == eREFOBJ) {
            u_.refObj->release();
        } else if (type_ == eARRAY) {
            SharedBuf<Any>::release(
                static_cast<SharedBuf<Any>::Bytes*>(u_.array));
        }
    }

private:

    struct RefObj : public RefCounted<RefObj> {
        template <typename T>
        inline static RefObj* from(const IntrusiveRefPtr<T>& ref)
        {
            return reinterpret_cast<RefObj*>(
                static_cast<RefCounted<T>*>(ref.get()));
        }
    };

    Type type_;
    union Data {
        void*   anyVal;
        int8_t  int8Val;
        int16_t int16Val;
        int32_t int32Val;
        int64_t int64Val;
        bool    boolVal;
        double  doubleVal;
        RefObj* refObj;
        Void*   array;
    } u_;
};

#define ANY_TYPE_DECL(T, t) \
    template<> struct Any::TypeInfo<T> { \
    typedef T Type; \
    static const Any::Type type = t; \
};
ANY_TYPE_DECL(int8_t,  Any::eINT8)
ANY_TYPE_DECL(int16_t, Any::eINT16)
ANY_TYPE_DECL(int32_t, Any::eINT32)
ANY_TYPE_DECL(int64_t, Any::eINT64)
ANY_TYPE_DECL(bool,    Any::eBOOL)
ANY_TYPE_DECL(double,  Any::eDOUBLE)
#undef ANY_TYPE_DECL

template <typename T>
inline T AnyCast(const Any& any)
{
    return AnyCastHelper<T>::cast(any);
}

template <typename T>
struct AnyCastHelper {
    typedef typename Any::TypeInfo<T>::Type Type;
    static inline Type cast(const Any& any)
    {
        return *reinterpret_cast<const T*>(&any.u_.anyVal);
    }
};

template <typename T>
struct AnyCastHelper< IntrusiveRefPtr<T> > {
    static inline IntrusiveRefPtr<T> cast(const Any& any)
    {
        assert( any.type_ == Any::eREFOBJ );
        return IntrusiveRefPtr<T>(
            reinterpret_cast<RefCounted<T>*>(any.u_.refObj));
    }
};

template<>
struct AnyCastHelper< SharedArray<Any> > {
    static inline SharedArray<Any> cast(const Any& any)
    {
        assert( any.type_ == Any::eARRAY );
        return SharedArray<Any>(
            static_cast<SharedBuf<Any>::Bytes*>(any.u_.array));
    }
};

#endif // ANY_H_INCLUDED