#include "boost/test/unit_test.hpp"

#include "future.h"

#include "pooled_mutex.h"
#include "static_pool_list.h"

#include <thread>
#include <functional>
namespace c11 {
    using namespace std;
}

#include <vector>

//-------------------------------------------

void promise_thread(ldl::Promise<int>* prom, int val) {
    c11::this_thread::sleep_for(std::chrono::seconds(2));
    prom->set_value(val);
}

BOOST_AUTO_TEST_SUITE(FUTURE)

BOOST_AUTO_TEST_CASE( future_test )
{
    BOOST_TEST_MESSAGE("Starting future_test");
    try {

        // let all pools grow as needed
        ldl::StaticPoolList::SetPoolGrowthStep(0, 10);

        ldl::SharedPointer<ldl::Promise<int>> prom_ptr(new ldl::Promise<int>());

        ldl::SharedPointer<ldl::Future<int>> fut_ptr(new ldl::Future<int>());

        *fut_ptr = prom_ptr->get_future();

        // spawn thread
        c11::thread th(c11::bind(promise_thread, prom_ptr.get(), 1));

        int val = fut_ptr->get();

        BOOST_CHECK_EQUAL(val, 1);

        if (th.joinable()) {
            th.join();
        }

        prom_ptr.reset();
        fut_ptr.reset();

        //repeat
        ldl::Promise<int> prom;

        ldl::Future<int> fut;
        fut.swap(prom.get_future());

        c11::thread th2(c11::bind(promise_thread, &prom, 2));

        int val2 = fut.get();

        BOOST_CHECK_EQUAL(val2, 2);

        if (th2.joinable()) {
            th2.join();
        }

    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in future-test: " << ex.what());
    }
}

BOOST_AUTO_TEST_SUITE_END()

