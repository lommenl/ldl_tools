#include "boost/test/unit_test.hpp"

#include "pool_allocator.h"
#include "static_pool_list.h"

BOOST_AUTO_TEST_SUITE(POOL_ALLOCATOR)

BOOST_AUTO_TEST_CASE( pool_allocator_test )
{
    BOOST_TEST_MESSAGE("Starting pool_allocator_test");

    try {
        // set growth_step for all pools to 10
        ldl::StaticPoolList::SetPoolGrowthStep(0, 10);

        typedef std::vector<int, ldl::PoolAllocator<int> > PoolVector;

        int* p1 = 0;
        {
            // construct vector that allocates from pool
            PoolVector v1 = PoolVector(ldl::PoolAllocator<int>());
            //resie vector to 10 elements
            v1.resize(10);
            BOOST_CHECK_EQUAL(v1.size(), 10);
            // save pointer to data
            p1 = v1.data();
        } // destroy v1 (and release memory back to pool)

        //construct vector that allocates from pool
        PoolVector v2 = PoolVector(ldl::PoolAllocator<int>());
        //resize vector to 10 elements
        v2.resize(10);
        BOOST_CHECK_EQUAL(v2.size(), 10);
        // verify pool memory is re-used
        int* p2 = v2.data();
        BOOST_CHECK_EQUAL(p1, p2);

    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in pool_allocator_test: " << ex.what());
    }

}
BOOST_AUTO_TEST_SUITE_END()
