#include "boost/test/unit_test.hpp"

#include "static_pool_list.h"

BOOST_AUTO_TEST_SUITE(STATIC_POOL_LIST)
BOOST_AUTO_TEST_CASE(static_pool_list_test)
{
    BOOST_TEST_MESSAGE("Starting static_pool_list_test");

    try {
        ldl::StaticPoolList::Reset();

        BOOST_CHECK_THROW(ldl::StaticPoolList::Pop(1), std::bad_alloc);

        ldl::StaticPoolList::SetPoolGrowthStep(0, 5);

        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolGrowthStep(1), 5);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolGrowthStep(10), 5);

        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetMaxPoolBlockSize(), static_cast<size_t>(1E9));

        BOOST_CHECK_EQUAL(ldl::StaticPoolList::PoolIsEmpty(1), true);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::PoolIsEmpty(10), true);

        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolSize(1), 0);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolSize(10), 0);

        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolFree(1), 0);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolFree(10), 0);

        ldl::StaticPoolList::SetPoolGrowthStep(1, 1);
        ldl::StaticPoolList::SetPoolGrowthStep(10, 10);

        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolGrowthStep(1), 1);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolGrowthStep(10), 10);

        BOOST_CHECK_EQUAL(ldl::StaticPoolList::HasPool(1), true);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::HasPool(3), false);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::HasPool(10), true);

        void* ptr_1a = nullptr;
        ptr_1a = ldl::StaticPoolList::Pop(1);
        BOOST_CHECK_NE(ptr_1a, nullptr);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolSize(1), 1);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolFree(1), 0);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::PoolIsEmpty(1), true);

        void* ptr_1b = nullptr;
        ptr_1b = ldl::StaticPoolList::Pop(1);
        BOOST_CHECK_NE(ptr_1b, nullptr);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolSize(1), 2);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolFree(1), 0);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::PoolIsEmpty(1), true);

        ldl::StaticPoolList::Push(1, ptr_1a);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolSize(1), 2);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolFree(1), 1);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::PoolIsEmpty(1), false);

        ldl::StaticPoolList::SetPoolGrowthStep(1, 0);

        void* ptr_1c = nullptr;
        ptr_1c = ldl::StaticPoolList::Pop(1);
        BOOST_CHECK_EQUAL(ptr_1a, ptr_1c);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolSize(1), 2);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::GetPoolFree(1), 0);
        BOOST_CHECK_EQUAL(ldl::StaticPoolList::PoolIsEmpty(1), true);

        BOOST_CHECK_THROW(ldl::StaticPoolList::Pop(1), std::bad_alloc);

        //---

        ldl::StaticPoolList static_pool;

        BOOST_CHECK_EQUAL(static_pool.HasPool(1), true);
        BOOST_CHECK_EQUAL(static_pool.HasPool(3), false);
        BOOST_CHECK_EQUAL(static_pool.HasPool(10), true);

        static_pool.IncreasePoolSize(10, 1);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(10), 1);
        BOOST_CHECK_EQUAL(static_pool.GetPoolGrowthStep(10), 10);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(10), 1);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(10), false);

        void* ptr_2a = nullptr;
        ptr_2a = static_pool.Pop(10);
        BOOST_CHECK_NE(ptr_2a, nullptr);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(10), 1);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(10), 0);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(10), true);

        void* ptr_2b = nullptr;
        ptr_2b = static_pool.Pop(10);
        BOOST_CHECK_NE(ptr_2b, nullptr);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(10), 9);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(10), false);

        static_pool.Push(10, ptr_2a);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(10), 10);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(10), false);

        static_pool.Push(10, ptr_2b);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(10), 11);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(10), false);

        void* ptr_2c = nullptr;
        ptr_2c = static_pool.Pop(10);
        BOOST_CHECK_EQUAL(ptr_2b, ptr_2c);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(10), 10);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(10), false);

        void* ptr_2d = nullptr;
        ptr_2d = static_pool.Pop(10);
        BOOST_CHECK_EQUAL(ptr_2d, ptr_2a);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(10), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(10), 9);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(10), false);

        static_pool.IncreasePoolSize(10, 100);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(10), 111);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(10), 109);

        static_pool.SetPoolGrowthStep(0, 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolGrowthStep(0), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolGrowthStep(3), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolGrowthStep(1), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolGrowthStep(10), 11);

        BOOST_CHECK_EQUAL(static_pool.HasPool(30), false);
        ldl::Pool& p30 = static_pool.GetPool(30);
        const ldl::Pool& cp30 = static_pool.GetPool(30);
        BOOST_CHECK_EQUAL(static_pool.HasPool(30), true);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(30), 0);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(30), 0);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(30), true);
        void* ptr_3a = 0;
        ptr_3a = p30.Pop();
        BOOST_CHECK_NE(ptr_3a, nullptr);
        BOOST_CHECK_EQUAL(static_pool.GetPoolSize(30), 11);
        BOOST_CHECK_EQUAL(static_pool.GetPoolFree(30), 10);
        BOOST_CHECK_EQUAL(static_pool.PoolIsEmpty(30), false);
        BOOST_CHECK_EQUAL(cp30.GetSize(), 11);
        BOOST_CHECK_EQUAL(cp30.GetFree(), 10);
        BOOST_CHECK_EQUAL(cp30.IsEmpty(), false);
        BOOST_CHECK_EQUAL(cp30.GetBlockSize(), 30);
    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in static_pool_list_test: " << ex.what());
    }
}
BOOST_AUTO_TEST_SUITE_END()
