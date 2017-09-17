#include "boost/test/unit_test.hpp"

#include "pool_list.h"

BOOST_AUTO_TEST_SUITE(POOL_LIST)
BOOST_AUTO_TEST_CASE(pool_list_test)
{
    BOOST_TEST_MESSAGE("Starting pool_list_test");

    try {
        ldl::PoolList plist;
        BOOST_CHECK_THROW(plist.Pop(1), std::bad_alloc);

        ldl::PoolList plist_2;
        plist_2.SetPoolGrowthStep(0, 5);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(1), 0);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(10), 0);
        BOOST_CHECK_EQUAL(plist_2.GetPoolGrowthStep(1), 5);
        BOOST_CHECK_EQUAL(plist_2.GetPoolGrowthStep(10), 5);

        plist.swap(plist_2);
        BOOST_CHECK_EQUAL(plist_2.GetPoolGrowthStep(1), 0);
        BOOST_CHECK_EQUAL(plist_2.GetPoolGrowthStep(10), 0);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(1), 5);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(10), 5);

        BOOST_CHECK_EQUAL(plist.GetMaxPoolBlockSize(), static_cast<size_t>(1E9));

        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(1), true);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(10), true);

        BOOST_CHECK_EQUAL(plist.GetPoolSize(1), 0);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10), 0);

        BOOST_CHECK_EQUAL(plist.GetPoolFree(1), 0);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 0);

        plist.SetPoolGrowthStep(1,1);
        plist.SetPoolGrowthStep(10,10);

        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(1), 1);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(10), 10);

        BOOST_CHECK_EQUAL(plist.HasPool(1), true);
        BOOST_CHECK_EQUAL(plist.HasPool(3), false);
        BOOST_CHECK_EQUAL(plist.HasPool(10), true);

        void* ptr_1a = nullptr;
        ptr_1a = plist.Pop(1);
        BOOST_CHECK_NE(ptr_1a, nullptr);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(1), 1);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(1), 0);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(1), true);

        void* ptr_1b = nullptr;
        ptr_1b = plist.Pop(1);
        BOOST_CHECK_NE(ptr_1b, nullptr);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(1), 2);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(1), 0);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(1), true);

        plist.Push(1,ptr_1a);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(1), 2);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(1), 1);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(1), false);

        plist.SetPoolGrowthStep(1,0);

        void* ptr_1c = nullptr;
        ptr_1c = plist.Pop(1);
        BOOST_CHECK_EQUAL(ptr_1a, ptr_1c);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(1), 2);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(1), 0);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(1), true);

        BOOST_CHECK_THROW(plist.Pop(1), std::bad_alloc);

        //---

        plist.IncreasePoolSize(10, 1);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10),1);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(10), 10);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 1);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(10), false);

        void* ptr_2a = nullptr;
        ptr_2a = plist.Pop(10);
        BOOST_CHECK_NE(ptr_2a, nullptr);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10), 1);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 0);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(10), true);

        void* ptr_2b = nullptr;
        ptr_2b = plist.Pop(10);
        BOOST_CHECK_NE(ptr_2b, nullptr);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 9);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(10), false);

        plist.Push(10,ptr_2a);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 10);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(10), false);

        plist.Push(10,ptr_2b);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 11);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(10), false);

        void* ptr_2c = nullptr;
        ptr_2c = plist.Pop(10);
        BOOST_CHECK_EQUAL(ptr_2b, ptr_2c);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 10);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(10), false);

        void* ptr_2d = nullptr;
        ptr_2d = plist.Pop(10);
        BOOST_CHECK_EQUAL(ptr_2d, ptr_2a);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 9);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(10), false);

        plist.IncreasePoolSize(10,100);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(10), 111);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(10), 109);

        plist.SetPoolGrowthStep(0,11);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(0), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(3), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(1), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolGrowthStep(10), 11);

        BOOST_CHECK_EQUAL(plist.HasPool(30), false);
        ldl::Pool& p30 = plist.GetPool(30);
        const ldl::Pool& cp30 = plist.GetPool(30);
        BOOST_CHECK_EQUAL(plist.HasPool(30), true);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(30), 0);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(30), 0);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(30), true);
        void* ptr_3a = 0;
        ptr_3a = p30.Pop();
        BOOST_CHECK_NE(ptr_3a, nullptr);
        BOOST_CHECK_EQUAL(plist.GetPoolSize(30), 11);
        BOOST_CHECK_EQUAL(plist.GetPoolFree(30), 10);
        BOOST_CHECK_EQUAL(plist.PoolIsEmpty(30), false);
        BOOST_CHECK_EQUAL(cp30.GetSize(), 11);
        BOOST_CHECK_EQUAL(cp30.GetFree(), 10);
        BOOST_CHECK_EQUAL(cp30.IsEmpty(), false);
        BOOST_CHECK_EQUAL(cp30.GetBlockSize(), 30);
    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in pool_list_test: " << ex.what());
    }
}
BOOST_AUTO_TEST_SUITE_END()
