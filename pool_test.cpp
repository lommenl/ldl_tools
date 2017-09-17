#include "boost/test/unit_test.hpp"

#include "pool.h"

BOOST_AUTO_TEST_SUITE(POOL)
BOOST_AUTO_TEST_CASE(pool_test)
{
    BOOST_TEST_MESSAGE("Starting pool_test");

    try {

        ldl::Pool pool_1;

        size_t block_size = 10;
        ldl::Pool pool_2(block_size);

        pool_1.swap(pool_2);

        BOOST_CHECK_EQUAL(pool_1.IsEmpty(), true);
        BOOST_CHECK_EQUAL(pool_2.IsEmpty(), true);

        BOOST_CHECK_EQUAL(pool_1.GetSize(),0 );
        BOOST_CHECK_EQUAL(pool_2.GetSize(),0 );

        BOOST_CHECK_EQUAL(pool_1.GetFree(),0 );
        BOOST_CHECK_EQUAL(pool_2.GetFree(),0 );

        BOOST_CHECK_EQUAL(pool_1.GetBlockSize(), 10);
        BOOST_CHECK_EQUAL(pool_2.GetBlockSize(), 0);

        BOOST_CHECK_EQUAL(pool_1.GetGrowthStep(), 0);
        BOOST_CHECK_EQUAL(pool_2.GetGrowthStep(), 0);

        pool_1.SetGrowthStep(1);
        pool_2.SetGrowthStep(5);

        BOOST_CHECK_EQUAL(pool_1.GetGrowthStep(), 1);
        BOOST_CHECK_EQUAL(pool_2.GetGrowthStep(), 5);

        void* ptr_1a = nullptr;
        ptr_1a = pool_1.Pop();
        BOOST_CHECK_NE(ptr_1a, nullptr);
        BOOST_CHECK_EQUAL(pool_1.GetSize(),1);
        BOOST_CHECK_EQUAL(pool_1.GetFree(),0);
        BOOST_CHECK_EQUAL(pool_1.IsEmpty(),true);

        void* ptr_1b = nullptr;
        ptr_1b = pool_1.Pop();
        BOOST_CHECK_NE(ptr_1b, nullptr);
        BOOST_CHECK_EQUAL(pool_1.GetSize(),2);
        BOOST_CHECK_EQUAL(pool_1.GetFree(),0);
        BOOST_CHECK_EQUAL(pool_1.IsEmpty(),true);

        pool_1.Push(ptr_1a);
        BOOST_CHECK_EQUAL(pool_1.GetSize(),2);
        BOOST_CHECK_EQUAL(pool_1.GetFree(),1);
        BOOST_CHECK_EQUAL(pool_1.IsEmpty(),false);

        pool_1.SetGrowthStep(0);

        void* ptr_1c = nullptr;
        ptr_1c = pool_1.Pop();
        BOOST_CHECK_EQUAL(ptr_1a, ptr_1c);
        BOOST_CHECK_EQUAL(pool_1.GetSize(),2);
        BOOST_CHECK_EQUAL(pool_1.GetFree(),0);
        BOOST_CHECK_EQUAL(pool_1.IsEmpty(),true);

        BOOST_CHECK_THROW(pool_1.Pop(),std::bad_alloc);


        //---

        pool_2.Initialize(2, 1, 10);
        BOOST_CHECK_EQUAL(pool_2.GetBlockSize(), 2);
        BOOST_CHECK_EQUAL(pool_2.GetSize(),1);
        BOOST_CHECK_EQUAL(pool_2.GetGrowthStep(),10);
        BOOST_CHECK_EQUAL(pool_2.GetFree(),1);
        BOOST_CHECK_EQUAL(pool_2.IsEmpty(),false);

        void* ptr_2a = nullptr;
        ptr_2a = pool_2.Pop();
        BOOST_CHECK_NE(ptr_2a, nullptr);
        BOOST_CHECK_EQUAL(pool_2.GetSize(),1);
        BOOST_CHECK_EQUAL(pool_2.GetFree(),0);
        BOOST_CHECK_EQUAL(pool_2.IsEmpty(),true);

        void* ptr_2b = nullptr;
        ptr_2b = pool_2.Pop();
        BOOST_CHECK_NE(ptr_2b, nullptr);
        BOOST_CHECK_EQUAL(pool_2.GetSize(),11);
        BOOST_CHECK_EQUAL(pool_2.GetFree(),9);
        BOOST_CHECK_EQUAL(pool_2.IsEmpty(),false);

        pool_2.Push(ptr_2a);
        BOOST_CHECK_EQUAL(pool_2.GetSize(),11);
        BOOST_CHECK_EQUAL(pool_2.GetFree(),10);
        BOOST_CHECK_EQUAL(pool_2.IsEmpty(),false);

        pool_2.Push(ptr_2b);
        BOOST_CHECK_EQUAL(pool_2.GetSize(),11);
        BOOST_CHECK_EQUAL(pool_2.GetFree(),11);
        BOOST_CHECK_EQUAL(pool_2.IsEmpty(),false);

        void* ptr_2c = nullptr;
        ptr_2c = pool_2.Pop();
        BOOST_CHECK_EQUAL(ptr_2b, ptr_2c);
        BOOST_CHECK_EQUAL(pool_2.GetSize(),11);
        BOOST_CHECK_EQUAL(pool_2.GetFree(),10);
        BOOST_CHECK_EQUAL(pool_2.IsEmpty(),false);

        void* ptr_2d = nullptr;
        ptr_2d = pool_2.Pop();
        BOOST_CHECK_EQUAL(ptr_2d, ptr_2a);
        BOOST_CHECK_EQUAL(pool_2.GetSize(),11);
        BOOST_CHECK_EQUAL(pool_2.GetFree(),9);
        BOOST_CHECK_EQUAL(pool_2.IsEmpty(),false);

        pool_2.IncreaseSize(100);
        BOOST_CHECK_EQUAL(pool_2.GetSize(),111);
        BOOST_CHECK_EQUAL(pool_2.GetFree(),109);

    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in pool_test: " << ex.what());
    }
}
BOOST_AUTO_TEST_SUITE_END()

