#include "boost/test/unit_test.hpp"

#include "pooled_new.h"

#include "shared_pointer.h"
#include "static_pool_list.h"

//-------------------------------------------

struct foo : public ldl::PooledNew<foo> {
    int64_t x;
};

BOOST_AUTO_TEST_SUITE(POOLED_NEW)
BOOST_AUTO_TEST_CASE( pooled_new_test )
{
    BOOST_TEST_MESSAGE("Starting pooled_new_test");
    try {
        ldl::StaticPoolList::Reset();

        // let all pools grow as needed
        ldl::StaticPoolList::SetPoolGrowthStep(0, 10);

        //except for the pool used by foo
        foo::SetPoolGrowthStep(1);
        BOOST_CHECK_EQUAL(foo::GetPoolGrowthStep(), 1);
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
            ldl::SharedPointer<foo> px4(new foo()); //allocates space for foo
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
        BOOST_CHECK_EQUAL(foo::GetArrayPoolSize(20), 0);
        BOOST_CHECK_EQUAL(foo::GetArrayPoolFree(20), 0);
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

    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in pooled_new_test: " << ex.what());
    }

}
BOOST_AUTO_TEST_SUITE_END()
