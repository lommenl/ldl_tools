#include "boost/test/unit_test.hpp"

#include "pool_allocator.h"

#include <iostream>

BOOST_AUTO_TEST_CASE( pool_allocator_test )
{

    std::cout << "============================" << std::endl;
    std::cout << "Starting pool_allocator_test" << std::endl;
    std::cout << "============================" << std::endl;

    try {
        // set growth_step for all pools to 10
        ldl::PoolAllocator<void>::SetPoolGrowthStep(0, 10);

        typedef std::vector<int, ldl::PoolAllocator<void> > PoolVector;

        int* p1 = 0;
        {
            PoolVector v1 = PoolVector(ldl::PoolAllocator<void>());
            v1.resize(10);
            BOOST_CHECK_EQUAL(v1.size(), 10);
            p1 = v1.data();
        }
        PoolVector v2 = PoolVector(ldl::PoolAllocator<void>());
        v2.resize(10);
        BOOST_CHECK_EQUAL(v2.size(), 10);
        int* p2 = v2.data();
        BOOST_CHECK_EQUAL(p1, p2);

        std::cout << "done" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "exception in pool_allocator_test: " << ex.what() << std::endl;
    }

}
