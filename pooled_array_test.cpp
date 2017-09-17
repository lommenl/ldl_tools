#include "boost/test/unit_test.hpp"

#include "pooled_array.h"
#include "pooled_new.h"
#include "static_pool_list.h"

#if 0 //FOOXXX
BOOST_AUTO_TEST_SUITE(POOLED_ARRAY)
BOOST_AUTO_TEST_CASE( pooled_array_test )
{
    BOOST_TEST_MESSAGE("Starting pooled_array_test");
    try {

        // let all pools grow as needed
        ldl::StaticPoolList::SetPoolGrowthStep(0, 10);

        ldl::PooledArray<uint8_t,100>  db;

        ldl::PooledArray<uint8_t,100>*  dbptr = new ldl::PooledArray<uint8_t,100>;

    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in pooled_array_test: " << ex.what());
    }

}
BOOST_AUTO_TEST_SUITE_END()
#endif //FOOXXX
