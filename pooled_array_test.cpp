#include "boost/test/unit_test.hpp"

#include "pooled_array.h"
#include "shared_pointer.h"

#include <iostream>

BOOST_AUTO_TEST_CASE( pooled_array_test )
{
    try {

        std::cout << "============================" << std::endl;
        std::cout << "Starting pooled_array_test" << std::endl;
        std::cout << "============================" << std::endl;

        // set uint8_t[10] pool to 1 element
        ldl::PooledArray<c11::uint8_t>::IncreasePoolSize(10,1);
        BOOST_CHECK_EQUAL(ldl::PooledArray<c11::uint8_t>::GetPoolSize(10), 1);
        BOOST_CHECK_EQUAL(ldl::PooledArray<c11::uint8_t>::GetPoolFree(10), 1);
        {
            ldl::PooledArray<c11::uint8_t> pb; // construct a 10 element array
            BOOST_CHECK_EQUAL(pb.size(), 0);
            BOOST_CHECK_EQUAL((bool)pb, false);
            BOOST_CHECK_EQUAL(ldl::PooledArray<c11::uint8_t>::GetPoolFree(10), 1);

            pb.SetSize(10);
            BOOST_CHECK_EQUAL(ldl::PooledArray<c11::uint8_t>::GetPoolFree(10), 0);
            BOOST_CHECK_EQUAL(pb.size(), 10);
            BOOST_CHECK_EQUAL((bool)pb, true);

            pb.fill(10);
            size_t count = 0;
            for (ldl::PooledArray<uint8_t>::const_iterator it = pb.begin(); it != pb.end(); ++it) {
                BOOST_CHECK_EQUAL(*it, 10);
                BOOST_CHECK_EQUAL(pb[count], 10);
                BOOST_CHECK_EQUAL(pb.at(count), 10);
                ++count;
            }
            BOOST_CHECK_EQUAL(count, 10);
        }
        BOOST_CHECK_EQUAL(ldl::PooledArray<c11::uint8_t>::GetPoolFree(10), 1);

        std::cout << "done" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "exception in pooled_array_test: " << ex.what() << std::endl;
    }

}
