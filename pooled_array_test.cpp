#include "boost/test/unit_test.hpp"

#include "pooled_array.h"
#include "pooled_new.h"
#include "static_pool_list.h"

BOOST_AUTO_TEST_SUITE(POOLED_ARRAY)
BOOST_AUTO_TEST_CASE( pooled_array_test )
{
    BOOST_TEST_MESSAGE("Starting pooled_array_test");
    try {

        // let all pools grow as needed
        ldl::StaticPoolList::SetPoolGrowthStep(0, 10);

        ldl::PooledArray<uint8_t, 100>  a1;
        ldl::PooledArray<uint8_t, 100>* pa2 = new ldl::PooledArray<uint8_t,100>;

        a1.fill(10);

        for (uint8_t ix = 0; ix < 100; ++ix) {
            (*pa2)[ix] = ix;
        }

        a1.swap(*pa2);

        BOOST_CHECK_EQUAL(a1.front(), 0);
        BOOST_CHECK_EQUAL(a1.at(10), 10);
        BOOST_CHECK_EQUAL(a1.back(), 99);

        BOOST_CHECK_EQUAL(pa2->front(), 10);
        BOOST_CHECK_EQUAL(pa2->at(10), 10);
        BOOST_CHECK_EQUAL(pa2->back(), 10);

        BOOST_CHECK_EQUAL(a1.size(), 100);
        BOOST_CHECK_EQUAL(a1.max_size(), 100);
        BOOST_CHECK_EQUAL(pa2->size(), 100);
        BOOST_CHECK_EQUAL(pa2->max_size(), 100);

        BOOST_CHECK_EQUAL(a1.empty(), false);
        BOOST_CHECK_EQUAL(pa2->empty(), false);

        BOOST_CHECK_NE(a1, *pa2);
        BOOST_CHECK_EQUAL(a1==*pa2,false);
        BOOST_CHECK_LT(a1, *pa2);
        BOOST_CHECK_EQUAL(*pa2<a1,false);
        BOOST_CHECK_LE(a1, *pa2);
        BOOST_CHECK_EQUAL(*pa2<=a1,false);
        BOOST_CHECK_GT(*pa2,a1);
        BOOST_CHECK_EQUAL(a1>*pa2,false);
        BOOST_CHECK_GE(*pa2,a1);
        BOOST_CHECK_EQUAL(a1 >= *pa2, false);

        uint8_t* dptr = pa2->data();
        delete pa2;
        ldl::PooledArray<uint8_t, 100>*  pa3 = new ldl::PooledArray<uint8_t,100>;

        BOOST_CHECK_EQUAL(dptr, pa3->data());

        uint8_t c = 0;
        for( ldl::PooledArray<uint8_t,100>::iterator it = pa3->begin(); it != pa3->end(); ++it) {
            *it = c++;
        }
        BOOST_CHECK_EQUAL(a1, *pa3);
        BOOST_CHECK_GE(a1, *pa3);
        BOOST_CHECK_LE(a1, *pa3);
        BOOST_CHECK_EQUAL(a1!=*pa3,false);
        BOOST_CHECK_EQUAL(a1>*pa3,false);
        BOOST_CHECK_EQUAL(a1<*pa3,false);

        /*
        db.GetArrayPoolFree;
        db.GetArrayPoolGrowthStep;
        db.GetArrayPoolIsEmpty;
        db.GetArrayPoolSize;
        db.GetPoolFree;
        db.GetPoolGrowthStep;
        db.GetPoolSize;
        db.IncreaseArrayPoolSize;
        db.IncreasePoolSize;
        db.operator delete;
        db.operator delete[];
        db.operator new;
        db.operator new[];
        db.PoolIsEmpty;
        db.SetArrayPoolGrowthStep;
        db.SetPoolGrowthStep;
        */

    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in pooled_array_test: " << ex.what());
    }

}
BOOST_AUTO_TEST_SUITE_END()
