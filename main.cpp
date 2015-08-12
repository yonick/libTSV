#include "WeakRef.h"
#include "Any.h"
#include "SharedArray.h"
#include <iostream>
#include <assert.h>
#include <stdint.h>

using namespace std;

struct NonEmpty {
    virtual int zzz(int) { return 0; }
    int yyy;
};

struct IFoo {
    virtual void foo() = 0;
};

struct IBar {
    virtual void bar() = 0;
};

struct FooBar
    : public NonEmpty
    , public IFoo
    , public IBar
    , public RefCounted<FooBar>
{
    int xxx;
    FooBar()
    {
        std::cout << "ctor of FooBar " << std::endl;
    }
    virtual ~FooBar()
    {
        std::cout << "dtor of FooBar " << std::endl;
    }
    virtual void foo()
    {
        std::cout << "calling foo" << std::endl;
    }
    virtual void bar()
    {
        std::cout << "calling bar" << std::endl;
    }
    virtual void cool()
    {
        std::cout << "calling cool" << std::endl;
    }
};

int main()
{
    IntrusiveRefPtr<FooBar> Foobar = new FooBar;
    {
        printf("test 0\n");
        RefPtr<IFoo> interfA = Foobar;
        interfA->foo();
        RefPtr<IBar> interfB = Foobar;
        interfB->bar();
    }

    {
        printf("test 1\n");
        Any anyArray = { 1, 0.1, { 2LL, Foobar } };
        SharedArray<Any> aa = AnyCast< SharedArray<Any> >(anyArray);
        SharedArray<Any> bb = AnyCast< SharedArray<Any> >(aa[2]);
        IntrusiveRefPtr<FooBar> dd = AnyCast< IntrusiveRefPtr<FooBar> >(bb[1]);
        dd->cool();
    }

    Foobar->cool();
    WeakRef<FooBar> weakFoobar(Foobar);

    {
        printf("test 2\n");
        RefPtr<FooBar> ptr = weakFoobar.lock();
        if (ptr) {
            ptr->cool();
            WeakRef<FooBar> weakFoobar2(ptr);
            RefPtr<FooBar> ptr2 = weakFoobar2.lock();
            ptr2->cool();
        }
    }

    return 0;
}