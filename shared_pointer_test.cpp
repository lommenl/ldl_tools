#include "boost/test/unit_test.hpp"

#include "shared_pointer.h"
#include "pooled_mutex.h"

#include <iostream>

//-------------------------------------------
BOOST_AUTO_TEST_CASE( shared_pointer_test )
{

    std::cout << "========================" << std::endl;
    std::cout << "Starting shared_ptr_test" << std::endl;
    std::cout << "========================" << std::endl;

    try {

        // create a pool of 10 PooledMutex objects for use by SharedPointer
        ldl::PooledMutex::IncreasePoolSize(10);

        // default constructor
        ldl::SharedPointer<int> s1;
        BOOST_CHECK_EQUAL(s1.use_count(), 0);
        BOOST_CHECK_EQUAL(s1.unique(), false);
        BOOST_CHECK_EQUAL((bool)s1, false);
        BOOST_CHECK_EQUAL(s1.get(), nullptr);

        //----

        // construct from pointer
        ldl::SharedPointer<int> s2(new int(11));
        BOOST_CHECK_EQUAL(s2.use_count(), 1);
        BOOST_CHECK_EQUAL(s2.unique(), true);
        BOOST_CHECK_EQUAL((bool)s2, true);
        BOOST_CHECK_EQUAL(s2.get()==nullptr, false);
        BOOST_CHECK_EQUAL(*s2, 11);

        //----

        // copy assignment
        s1 = s2;
        BOOST_CHECK_EQUAL(s1.use_count(), 2);
        BOOST_CHECK_EQUAL(s1.unique(), false);
        BOOST_CHECK_EQUAL((bool)s1, true);
        BOOST_CHECK_EQUAL(s1.get(), s2.get());
        BOOST_CHECK_EQUAL(*s1, 11);

        BOOST_CHECK_EQUAL(s2.use_count(), 2);
        BOOST_CHECK_EQUAL(s2.unique(), false);
        BOOST_CHECK_EQUAL((bool)s2, true);
        BOOST_CHECK_EQUAL(s1.get(), s2.get());
        BOOST_CHECK_EQUAL(*s2, 11);

        //----

        // reset(ptr)
        s2.reset(new int(-10));
        BOOST_CHECK_EQUAL(s1.use_count(), 1);
        BOOST_CHECK_EQUAL(s1.unique(), true);
        BOOST_CHECK_EQUAL((bool)s1, true);
        BOOST_CHECK_EQUAL(s1.get()==s2.get(),false);
        BOOST_CHECK_EQUAL(*s1, 11);

        BOOST_CHECK_EQUAL(s2.use_count(), 1);
        BOOST_CHECK_EQUAL(s2.unique(), true);
        BOOST_CHECK_EQUAL((bool)s2, true);
        BOOST_CHECK_EQUAL(s1.get()==s2.get(),false);
        BOOST_CHECK_EQUAL(*s2, -10);

        //----

        // reset();
        s1.reset();
        BOOST_CHECK_EQUAL(s1.use_count(), 0);
        BOOST_CHECK_EQUAL(s1.unique(), false);
        BOOST_CHECK_EQUAL((bool)s1, false);
        BOOST_CHECK_EQUAL(s1.get(), nullptr);

        BOOST_CHECK_EQUAL(s2.use_count(), 1);
        BOOST_CHECK_EQUAL(s2.unique(), true);
        BOOST_CHECK_EQUAL((bool)s2, true);
        BOOST_CHECK_EQUAL(s1.get()==s2.get(),false);
        BOOST_CHECK_EQUAL(*s2, -10);
        // -----

        // copy constructor
        ldl::SharedPointer<int> s3(s2);

        BOOST_CHECK_EQUAL(s1.use_count(), 0);
        BOOST_CHECK_EQUAL(s1.unique(), false);
        BOOST_CHECK_EQUAL((bool)s1, false);
        BOOST_CHECK_EQUAL(s1.get(), nullptr);

        BOOST_CHECK_EQUAL(s2.use_count(), 2);
        BOOST_CHECK_EQUAL(s2.unique(), false);
        BOOST_CHECK_EQUAL((bool)s2, true);
        BOOST_CHECK_EQUAL(s2.get(), s3.get());
        BOOST_CHECK_EQUAL(*s2, -10);

        BOOST_CHECK_EQUAL(s3.use_count(), 2);
        BOOST_CHECK_EQUAL(s3.unique(), false);
        BOOST_CHECK_EQUAL((bool)s3, true);
        BOOST_CHECK_EQUAL(s2.get(), s3.get());
        BOOST_CHECK_EQUAL(*s3, -10);

        // -----
        // swap
        s1.swap(s3);
        BOOST_CHECK_EQUAL(s3.use_count(), 0);
        BOOST_CHECK_EQUAL(s3.unique(), false);
        BOOST_CHECK_EQUAL((bool)s3, false);
        BOOST_CHECK_EQUAL(s3.get(), nullptr);

        BOOST_CHECK_EQUAL(s2.use_count(), 2);
        BOOST_CHECK_EQUAL(s2.unique(), false);
        BOOST_CHECK_EQUAL((bool)s2, true);
        BOOST_CHECK_EQUAL(s2.get(), s1.get());
        BOOST_CHECK_EQUAL(*s2, -10);

        BOOST_CHECK_EQUAL(s1.use_count(), 2);
        BOOST_CHECK_EQUAL(s1.unique(), false);
        BOOST_CHECK_EQUAL((bool)s1, true);
        BOOST_CHECK_EQUAL(s2.get(), s1.get());
        BOOST_CHECK_EQUAL(*s1, -10);

        // -----
        //auto delete
        {
            ldl::SharedPointer<int> s4(0); // construct w/ nullptr
            BOOST_CHECK_EQUAL(s4.use_count(), 0);
            BOOST_CHECK_EQUAL(s4.unique(), false);
            BOOST_CHECK_EQUAL((bool)s4, false);

            s4 = s1;
            BOOST_CHECK_EQUAL(s4.use_count(), 3);
            BOOST_CHECK_EQUAL(s4.unique(), false);
            BOOST_CHECK_EQUAL((bool)s4, true);
            BOOST_CHECK_EQUAL(s4.get(), s1.get());
            BOOST_CHECK_EQUAL(*s4, -10);

            BOOST_CHECK_EQUAL(s1.use_count(), 3);
            BOOST_CHECK_EQUAL(s1.unique(), false);
            BOOST_CHECK_EQUAL((bool)s1, true);
            BOOST_CHECK_EQUAL(s4.get(), s1.get());
            BOOST_CHECK_EQUAL(*s1, -10);
        }

        BOOST_CHECK_EQUAL(s1.use_count(), 2);
        BOOST_CHECK_EQUAL(s1.unique(), false);
        BOOST_CHECK_EQUAL((bool)s1, true);
        BOOST_CHECK_EQUAL(*s1, -10);

        // -----
        // TODO:
        // SharedPointer(ptr,del)
        // operator=(SharedPointer<U,M>&)
        // reset(ptr)
        // reset(ptr,del)
        // operator->()
        //----
        // swap(lhs,rhs)
        // operator==(lhs,rhs);
        // operator!=(lhs,rhs);
        // operator<(lhs,rhs);
        // operator>(lhs,rhs);
        // operator<=(lhs,rhs);
        // operator=>(lhs,rhs);

        std::cout << "done" << std::endl;

    }
    catch (const std::exception& ex) {
        std::cout << "exception in shared_pointer_test: " << ex.what() << std::endl;
    }
}
