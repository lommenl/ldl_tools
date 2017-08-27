#include "boost/test/unit_test.hpp"

#include "pooled_new.h"
#include "shared_pointer.h"

#include <iostream>

//-------------------------------------------

struct foo : public ldl::PooledNew<foo> {
    int x;
};

BOOST_AUTO_TEST_CASE( pooled_new_test )
{
    try {

        std::cout << "============================" << std::endl;
        std::cout << "Starting pooled_new_test" << std::endl;
        std::cout << "============================" << std::endl;

        foo::SetPoolGrowthStep(1);
        BOOST_CHECK_EQUAL(foo::GetPoolGrowthStep(), 1);

        foo::IncreasePoolSize(1);
        BOOST_CHECK_EQUAL(foo::GetPoolSize(), 1);
        BOOST_CHECK_EQUAL(foo::GetPoolFree(), 1);

        foo x1;
        x1.x = 11;
        BOOST_CHECK_EQUAL(x1.x, 11);
        BOOST_CHECK_EQUAL(foo::GetPoolSize(), 1);
        BOOST_CHECK_EQUAL(foo::GetPoolFree(), 1);


        foo* px2 = new foo();
        px2->x = 22;
        BOOST_CHECK_EQUAL(px2->x, 22);
        BOOST_CHECK_EQUAL(foo::GetPoolSize(), 1);
        BOOST_CHECK_EQUAL(foo::GetPoolFree(), 0);

        foo* px3 = new foo();
        px3->x = 33;
        BOOST_CHECK_EQUAL(px3->x, 33);
        BOOST_CHECK_EQUAL(px3!=px2,true);
        BOOST_CHECK_EQUAL(foo::GetPoolSize(), 2);
        BOOST_CHECK_EQUAL(foo::GetPoolFree(), 0);

        delete px2;
        BOOST_CHECK_EQUAL(foo::GetPoolSize(), 2);
        BOOST_CHECK_EQUAL(foo::GetPoolFree(), 1);

        {
            ldl::SharedPointer<foo> px4(new foo());
            px4->x = 44;
            BOOST_CHECK_EQUAL(px4->x, 44);
            BOOST_CHECK_EQUAL(px4.get(),px2);
            BOOST_CHECK_EQUAL(foo::GetPoolSize(), 2);
            BOOST_CHECK_EQUAL(foo::GetPoolFree(), 0);
        }
        BOOST_CHECK_EQUAL(foo::GetPoolSize(), 2);
        BOOST_CHECK_EQUAL(foo::GetPoolFree(), 1);

        ldl::SharedPointer<foo> px5(new foo());
        px5->x = 55;
        BOOST_CHECK_EQUAL(px5->x, 55);
        BOOST_CHECK_EQUAL(px5.get(),px2);
        BOOST_CHECK_EQUAL(foo::GetPoolSize(), 2);
        BOOST_CHECK_EQUAL(foo::GetPoolFree(), 0);

        //------------------

        // allocate space for 10 foo[20] arrays.
        foo::IncreaseArrayPoolSize(20, 10);
        BOOST_CHECK_EQUAL(foo::GetArrayPoolSize(20), 10);
        BOOST_CHECK_EQUAL(foo::GetArrayPoolFree(20), 10);

        foo* a1 = new foo[20];
        BOOST_CHECK_EQUAL(foo::GetArrayPoolSize(20), 10);
        BOOST_CHECK_EQUAL(foo::GetArrayPoolFree(20), 9);

        BOOST_CHECK_EQUAL(a1 == 0, false);

        for (int ix = 0; ix < 20; ++ix) {
            a1[ix].x = 100+ix;
            BOOST_CHECK_EQUAL(&a1[ix], a1 + ix);
            BOOST_CHECK_EQUAL(a1[ix].x, 100+ix);
        }

        delete[] a1;
        BOOST_CHECK_EQUAL(foo::GetArrayPoolSize(20), 10);
        BOOST_CHECK_EQUAL(foo::GetArrayPoolFree(20), 10);

        std::cout << "done" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "exception in pooled_new_test: " << ex.what() << std::endl;
    }

}
