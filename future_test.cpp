#include "boost/test/unit_test.hpp"

#include "future.h"

#include <thread>
#include <functional>
namespace c11 {
    using namespace std;
}

#include <iostream>
#include <vector>

//-------------------------------------------

void promise_thread(ldl::Promise<int>* prom, int val) {
    c11::this_thread::sleep_for(std::chrono::seconds(2));
    prom->set_value(val);
}

BOOST_AUTO_TEST_CASE( future_test )
{
    std::cout << "============================" << std::endl;
    std::cout << "Starting future_test" << std::endl;
    std::cout << "============================" << std::endl;

    try {

        ldl::FutureState<int> state;

        ldl::Promise<int> prom(&state);

        ldl::Future<int> fut;
        fut.swap(prom.get_future()); // move assignment

        // spawn thread
        c11::thread th(c11::bind(promise_thread, &prom, 1));

        int val = fut.get();

        BOOST_CHECK_EQUAL(val, 1);

        th.join();

        //repeat
        ldl::Promise<int> prom2(&state);

        ldl::Future<int> fut2;
        fut2.swap(prom2.get_future()); // move assignment

        c11::thread th2(c11::bind(promise_thread, &prom2, 2));

        int val2 = fut2.get();

        BOOST_CHECK_EQUAL(val2, 2);

        th2.join();

        std::cout << "done" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "exception in future-test: " << ex.what() << std::endl;
    }
}
