#include "boost/test/unit_test.hpp"

#include "static_pool_list.h"

BOOST_AUTO_TEST_SUITE(STATIC_POOL_LIST)
BOOST_AUTO_TEST_CASE(static_pool_list_test)
{
    BOOST_TEST_MESSAGE("Starting static_pool_list_test");

    try {

        //BOOST_CHECK_EQUAL(pool_2.GetFree(),109);

    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in static_pool_list_test: " << ex.what());
    }
}
BOOST_AUTO_TEST_SUITE_END()
