#include "boost/test/unit_test.hpp"

#include "future.h"

#include "pooled_mutex.h"
#include "pool_allocator.h"

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
    try {

        std::cout << "============================" << std::endl;
        std::cout << "Starting future_test" << std::endl;
        std::cout << "============================" << std::endl;

        // let all pools grow as needed
        ldl::PoolAllocator<void>::SetPoolGrowthStep(0, 10);

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

        std::cout << "done" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "exception in future-test: " << ex.what() << std::endl;
    }
}
