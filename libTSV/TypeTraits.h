#ifndef TYPETRIATS_H_INCLUDED
#define TYPETRIATS_H_INCLUDED

#include <assert.h>

/* Conditional */
template <bool b, typename A, typename B>
struct Conditional {
  typedef A Type;
};

template <typename A, typename B>
struct Conditional<false, A, B> {
  typedef B Type;
};

/* EnableIf */
template <bool b, typename T>
struct EnableIf {};

template <typename T>
struct EnableIf<true, T> {
  typedef T Type;
};

/* foundamantal */
template <typename T, T Value>
struct ConstantValue {
    static const T value = Value;
    typedef T ValueType;
    typedef ConstantValue<T, Value> Type;
};

template <bool b>
struct BooleanType : ConstantValue<bool, b> {};

/* RemoveConst */
template<typename T>
struct RemoveConst {
  typedef T Type;
};

template<typename T>
struct RemoveConst<const T> {
  typedef T Type;
};

/* RemoveVolatile */
template<typename T>
struct RemoveVolatile {
  typedef T Type;
};

template<typename T>
struct RemoveVolatile<volatile T> {
  typedef T Type;
};

/* RemoveConstVolatile */
template <typename T>
struct RemoveConstVolatile {
    typedef typename RemoveVolatile<typename RemoveConst<T>::Type>::Type Type;
};

/* RemovePointer */
template <typename T>
struct RemovePointer {
    typedef T Type;
};

template <typename T>
struct RemovePointer<T*> {
    typedef T Type;
};

/* RemoveReference */
template <typename T>
struct RemoveReference {
    typedef T Type;
};

template <typename T>
struct RemoveReference<T&> {
    typedef T Type;
};

/* RemoveExtent */
template <typename T>
struct RemoveExtent {
    typedef T Type;
};

template <typename T>
struct RemoveExtent<T[]> {
    typedef T Type;
};

template <typename T, size_t N>
struct RemoveExtent<T[N]> {
    typedef T Type;
};

/* IsIntergral */
namespace detail {

template <typename T>
struct IsIntegralHelper : BooleanType<false> {};

template<> struct IsIntegralHelper<char>               : BooleanType<true> {};
template<> struct IsIntegralHelper<signed char>        : BooleanType<true> {};
template<> struct IsIntegralHelper<unsigned char>      : BooleanType<true> {};
template<> struct IsIntegralHelper<short>              : BooleanType<true> {};
template<> struct IsIntegralHelper<unsigned short>     : BooleanType<true> {};
template<> struct IsIntegralHelper<int>                : BooleanType<true> {};
template<> struct IsIntegralHelper<unsigned int>       : BooleanType<true> {};
template<> struct IsIntegralHelper<long>               : BooleanType<true> {};
template<> struct IsIntegralHelper<unsigned long>      : BooleanType<true> {};
template<> struct IsIntegralHelper<long long>          : BooleanType<true> {};
template<> struct IsIntegralHelper<unsigned long long> : BooleanType<true> {};
template<> struct IsIntegralHelper<bool>               : BooleanType<true> {};
template<> struct IsIntegralHelper<wchar_t>            : BooleanType<true> {};

} // namespace detail

template<typename T>
struct IsIntegral 
    : detail::IsIntegralHelper<
        typename RemoveConstVolatile<T>::Type>
{};

/* IsFloatingPoint */
namespace detail {

template <typename T>
struct IsFloatingPointHelper : BooleanType<false> {};

template<> struct IsFloatingPointHelper<float>       : BooleanType<true> {};
template<> struct IsFloatingPointHelper<double>      : BooleanType<true> {};
template<> struct IsFloatingPointHelper<long double> : BooleanType<true> {};

} // namespace detail

template<typename T>
struct IsFloatingPoint
    : detail::IsFloatingPointHelper<
        typename RemoveConstVolatile<T>::Type>
{};

/* IsAtrithmetic */
template<typename T>
struct IsArithmetic
    : BooleanType<IsIntegral<T>::value || IsFloatingPoint<T>::value>
{};

/* IsPod */
template<typename T>
struct IsPod : public BooleanType<false> {};

template<> struct IsPod<char>               : BooleanType<true> {};
template<> struct IsPod<signed char>        : BooleanType<true> {};
template<> struct IsPod<unsigned char>      : BooleanType<true> {};
template<> struct IsPod<short>              : BooleanType<true> {};
template<> struct IsPod<unsigned short>     : BooleanType<true> {};
template<> struct IsPod<int>                : BooleanType<true> {};
template<> struct IsPod<unsigned int>       : BooleanType<true> {};
template<> struct IsPod<long>               : BooleanType<true> {};
template<> struct IsPod<unsigned long>      : BooleanType<true> {};
template<> struct IsPod<long long>          : BooleanType<true> {};
template<> struct IsPod<unsigned long long> : BooleanType<true> {};
template<> struct IsPod<bool>               : BooleanType<true> {};
template<> struct IsPod<float>              : BooleanType<true> {};
template<> struct IsPod<double>             : BooleanType<true> {};
template<> struct IsPod<wchar_t>            : BooleanType<true> {};

template<typename T> struct IsPod<T*>       : BooleanType<true> {};

/* IsSame */
template<typename T, typename U>
struct IsSame : BooleanType<false> {};

template<typename T>
struct IsSame<T, T> : BooleanType<true> {};

/* IsConvertible */
namespace detail {

template <typename From, typename To>
struct ConvertibleTester
{
private:
    static From create();

    template<typename F, typename T> static char test(T);
    template<typename F, typename T> static long test(...);
public:
    static const bool value =
        sizeof(test<From, To>(create())) == sizeof(char);
};

} // namespace detail

template<typename From, typename To>
struct IsConvertible : BooleanType<detail::ConvertibleTester<From, To>::value> {};

/* STATIC_ASSERT */
template <bool x>
struct __STATIC_ASSERTION_FAILURE;

template <>
struct __STATIC_ASSERTION_FAILURE<true> {
    enum { value = 1 };
};

template <int x> 
struct __STATIC_ASSERT_TEST {
};

#define PRECOMPILE_COMBINE(a,b) a#b

#if defined(__GNUC__) && (__GNUC__ == 3) && ((__GNUC_MINOR__ == 3) || (__GNUC_MINOR__ == 4))
#define STATIC_ASSERT(s) \
	typedef char PRECOMPILE_COMBINE(static_assert_error_in_, __LINE__) \
        [__STATIC_ASSERTION_FAILURE<(bool) (s)>::value]
#else
#define STATIC_ASSERT(s) \
	typedef __STATIC_ASSERT_TEST<sizeof(__STATIC_ASSERTION_FAILURE<(bool) (s)>)> \
		PRECOMPILE_COMBINE(static_assert_error_in_, __LINE__)
#endif

/* RUNTIME_ASSERT */
#define RUNTIME_ASSERT(s) assert((s) && !!#s)

#endif // TYPETRIATS_H_INCLUDED