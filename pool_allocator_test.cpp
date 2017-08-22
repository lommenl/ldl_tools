#include "boost/test/unit_test.hpp"

#include "pool_allocator.h"
#include "shared_pointer.h"

#include <iostream>

BOOST_AUTO_TEST_CASE( pool_allocator_test )
{

    std::cout << "============================" << std::endl;
    std::cout << "Starting pool_allocator_test" << std::endl;
    std::cout << "============================" << std::endl;

    try {
        // initialize 4-byte pool with 1 element
        ldl::PoolAllocator<int>::IncreasePoolSize(1,1); //FIXME

        // set growth_step for all pools to 10
        ldl::PoolAllocator<int>::SetPoolGrowthStep(0, 10); //FIXME

        // allocate 1st element
        ldl::SharedPointer<int> p1 = ldl::PoolAllocator<int>::New();
        int* p1chk = p1.get();
        BOOST_CHECK_EQUAL(*p1, 0);

        // allocate 2nd element from 4-byte pool (causes pool to resize)
        ldl::SharedPointer<short> p2 = ldl::PoolAllocator<short>::NewArray(2, 3);
        short* p2chk = p2.get();
        BOOST_CHECK_EQUAL(p2chk[0], 3);
        BOOST_CHECK_EQUAL(p2chk[1], 3);
        BOOST_CHECK_EQUAL(*p1, 0);

        //delete 1st element
        p1.reset();

        // allocate 3rd element from 4-byte pool (re-use memory from p1)
        ldl::SharedPointer<int> p3 = ldl::PoolAllocator<int>::New(11);
        int* p3chk = p3.get();
        BOOST_CHECK_EQUAL(p1chk, p3chk);
        BOOST_CHECK_EQUAL(*p3, 11);

        // delete p2 and p3
        p3.reset();
        p2.reset();

        int* p4chk;
        {
            ldl::SharedPointer<int> p4 = ldl::PoolAllocator<int>::New(10);
            p4chk = p4.get();
            BOOST_CHECK_EQUAL((void*)p4chk, (void*)p2chk);
            BOOST_CHECK_EQUAL(*p4, 10);
        } // auto-delete p4

        ldl::SharedPointer<int> p5 = ldl::PoolAllocator<int>::New(99);
        int* p5chk = p5.get();
        BOOST_CHECK_EQUAL(p4chk, p5chk);
        BOOST_CHECK_EQUAL(*p5, 99);

        //---------------

        // see if we can use it as an allocator for vectors

        typedef std::vector<int, ldl::PoolAllocator<int> > PoolVector;
        PoolVector v = PoolVector(ldl::PoolAllocator<int>());

        v.resize(10);

        BOOST_CHECK_EQUAL(v.size(), 10);

        std::cout << "done" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "exception in pool_allocator_test: " << ex.what() << std::endl;
    }

}
