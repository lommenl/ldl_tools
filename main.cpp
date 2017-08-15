#include <iostream>

#include "shared_pointer.h"
#include "pool_allocator.h"
#include "future.h"

#include <thread>
#include <functional>

#include <vector>

//-------------------------------------------
void shared_ptr_test() {

    std::cout << "========================" << std::endl;
    std::cout << "Starting shared_ptr_test" << std::endl;
    std::cout << "========================" << std::endl;

    c11::mutex mx;

    try {
        // default constructor
        ldl::SharedPointer<int> s1;
        std::cout << "ldl::SharedPointer<int> s1;" << std::endl;
        std::cout << "s1.use_count() = " << s1.use_count() << std::endl;
        std::cout << "s1.unique() = " << s1.unique() << std::endl;
        std::cout << "(bool)s1=" << (bool)s1 << std::endl;
        std::cout << "s1.get() = " << s1.get() << std::endl;

        //----

        // construct from pointer
        ldl::SharedPointer<int> s2(new int(11));
        std::cout << "ldl::SharedPointer<int> s2(new int(11));" << std::endl;
        std::cout << "s2.use_count() = " << s2.use_count() << std::endl;
        std::cout << "s2.unique() = " << s2.unique() << std::endl;
        std::cout << "(bool)s2=" << (bool)s2 << std::endl;
        std::cout << "s2.get() = " << s2.get() << std::endl;
        std::cout << "*s2 = " << *s2 << std::endl;

        //----

        // copy assignment
        s1 = s2;
        std::cout << "s1 = s2;" << std::endl;
        std::cout << "s1.use_count() = " << s1.use_count() << std::endl;
        std::cout << "s1.unique() = " << s1.unique() << std::endl;
        std::cout << "(bool)s1=" << (bool)s1 << std::endl;
        std::cout << "s1.get() = " << s1.get() << std::endl;
        std::cout << "*s1 = " << *s1 << std::endl;

        std::cout << "s2.use_count() = " << s2.use_count() << std::endl;
        std::cout << "s2.unique() = " << s2.unique() << std::endl;
        std::cout << "(bool)s2=" << (bool)s2 << std::endl;
        std::cout << "s2.get() = " << s2.get() << std::endl;
        std::cout << "*s2 = " << *s2 << std::endl;

        //----

        // reset(ptr)
        s2.reset(new int(-10), mx);
        std::cout << "s2.reset(new int(-10), mx);" << std::endl;

        std::cout << "s1.use_count() = " << s1.use_count() << std::endl;
        std::cout << "s1.unique() = " << s1.unique() << std::endl;
        std::cout << "(bool)s1=" << (bool)s1 << std::endl;
        std::cout << "s1.get() = " << s1.get() << std::endl;
        std::cout << "*s1 = " << *s1 << std::endl;

        std::cout << "s2.use_count() = " << s2.use_count() << std::endl;
        std::cout << "s2.unique() = " << s2.unique() << std::endl;
        std::cout << "(bool)s2=" << (bool)s2 << std::endl;
        std::cout << "s2.get() = " << s2.get() << std::endl;
        std::cout << "*s2 = " << *s2 << std::endl;

        //----

        // reset();
        s1.reset();
        std::cout << "s1.reset();" << std::endl;
        std::cout << "s1.use_count() = " << s1.use_count() << std::endl;
        std::cout << "s1.unique() = " << s1.unique() << std::endl;
        std::cout << "(bool)s1=" << (bool)s1 << std::endl;
        std::cout << "s1.get() = " << s1.get() << std::endl;

        std::cout << "s2.use_count() = " << s2.use_count() << std::endl;
        std::cout << "s2.unique() = " << s2.unique() << std::endl;
        std::cout << "(bool)s2=" << (bool)s2 << std::endl;
        std::cout << "s2.get() = " << s2.get() << std::endl;
        std::cout << "*s2 = " << *s2 << std::endl;

        // -----

        // copy constructor
        ldl::SharedPointer<int> s3(s2);
        std::cout << "ldl::SharedPointer<int> s3(s2);" << std::endl;

        std::cout << "s1.use_count() = " << s1.use_count() << std::endl;
        std::cout << "s1.unique() = " << s1.unique() << std::endl;
        std::cout << "(bool)s1=" << (bool)s1 << std::endl;
        std::cout << "s1.get() = " << s1.get() << std::endl;
        std::wcout << "s1 = " << s1 << std::endl;

        std::cout << "s2.use_count() = " << s2.use_count() << std::endl;
        std::cout << "s2.unique() = " << s2.unique() << std::endl;
        std::cout << "(bool)s2=" << (bool)s2 << std::endl;
        std::cout << "s2.get() = " << s2.get() << std::endl;
        std::cout << "*s2 = " << *s2 << std::endl;

        std::cout << "s3.use_count() = " << s3.use_count() << std::endl;
        std::cout << "s3.unique() = " << s3.unique() << std::endl;
        std::cout << "(bool)s3=" << (bool)s3 << std::endl;
        std::cout << "s3.get() = " << s3.get() << std::endl;
        std::cout << "*s3 = " << *s3 << std::endl;

        // -----
        // swap
        s1.swap(s3);
        std::cout << "s1.swap(s3);" << std::endl;

        std::cout << "s1.use_count() = " << s1.use_count() << std::endl;
        std::cout << "s1.unique() = " << s1.unique() << std::endl;
        std::cout << "(bool)s1=" << (bool)s1 << std::endl;
        std::cout << "s1.get() = " << s1.get() << std::endl;
        std::cout << "*s1 = " << *s1 << std::endl;

        std::cout << "s2.use_count() = " << s2.use_count() << std::endl;
        std::cout << "s2.unique() = " << s2.unique() << std::endl;
        std::cout << "(bool)s2=" << (bool)s2 << std::endl;
        std::cout << "s2.get() = " << s2.get() << std::endl;
        std::cout << "*s2 = " << *s2 << std::endl;

        std::cout << "s3.use_count() = " << s3.use_count() << std::endl;
        std::cout << "s3.unique() = " << s3.unique() << std::endl;
        std::cout << "(bool)s3=" << (bool)s3 << std::endl;
        std::cout << "s3.get() = " << s3.get() << std::endl;

        // -----
        //auto delete
        {
            std::cout << "entering block" << std::endl;
            ldl::SharedPointer<int> s4(0);
            std::cout << "ldl::SharedPointer<int> s4(0);" << std::endl;
            std::cout << "s4.use_count() = " << s4.use_count() << std::endl;
            std::cout << "s4.unique() = " << s4.unique() << std::endl;
            std::cout << "(bool)s4=" << (bool)s4 << std::endl;
            std::cout << "s4 = " << s4 << std::endl;
            s4 = s1;
            std::cout << "s4 = s1;" << std::endl;
            std::cout << "s1.use_count() = " << s1.use_count() << std::endl;
            std::cout << "s1.unique() = " << s1.unique() << std::endl;
            std::cout << "(bool)s1=" << (bool)s1 << std::endl;
            std::cout << "s1 = " << s1 << std::endl;
            std::cout << "*s1 = " << *s4 << std::endl;

            std::cout << "s4.use_count() = " << s4.use_count() << std::endl;
            std::cout << "s4.unique() = " << s4.unique() << std::endl;
            std::cout << "(bool)s4=" << (bool)s4 << std::endl;
            std::cout << "s4 = " << s4 << std::endl;
            std::cout << "*s4 = " << *s4 << std::endl;

            std::cout << "exiting block" << std::endl;
        }

        std::cout << "s1.use_count() = " << s1.use_count() << std::endl;
        std::cout << "s1.unique() = " << s1.unique() << std::endl;
        std::cout << "(bool)s1=" << (bool)s1 << std::endl;
        std::cout << "s1 = " << s1 << std::endl;
        std::cout << "*s1 = " << *s1 << std::endl;

        // -----
        // TODO:
        // SharedPointer(ptr,mx)
        // SharedPointer(ptr,del)
        // SharedPointer(ptr,del,mx)
        // operator=(SharedPointer<U,M>&)
        // reset(ptr,mx)
        // reset(ptr,del)
        // reset(ptr,del,mx)
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
        std::cout << "exception in shared_ptr_test: " << ex.what() << std::endl;
    }
}

//-------------------------------------------
void pool_allocator_test() {

    std::cout << "============================" << std::endl;
    std::cout << "Starting pool_allocator_test" << std::endl;
    std::cout << "============================" << std::endl;

    try {
        // initialize 4-byte pool with 1 element
        ldl::PoolAllocator<int>::ResizePool(1,1);

        // allocate 1st element
        ldl::SharedPointer<int> p1 = ldl::PoolAllocator<int>::New();
        std::cout << "p1 = " << p1 << std::endl;
        std::cout << "*p1 = " << *p1 << std::endl;

        // allocate 2nd element from 4-byte pool (causes pool to resize)
        ldl::SharedPointer<short> p2 = ldl::PoolAllocator<short>::NewArray(2,3);
        std::cout << "p2 = " << p2 << std::endl;
        std::cout << "*p2 = " << *p2 << std::endl;

        //delete 1st element
        p1.reset();

        // allocate 3rd element from 4-byte pool (re-use memory from p1)
        ldl::SharedPointer<int> p3 = ldl::PoolAllocator<int>::New(11);
        std::cout << "p3 = " << p3 << std::endl;
        std::cout << "*p3 = " << *p3 << std::endl;

        // delete p2 and p3
        p3.reset();
        p2.reset();

        {
            ldl::SharedPointer<int> p4 = ldl::PoolAllocator<int>::New(10);
            std::cout << "p4 =" << p4 << std::endl;
            std::cout << "*p4 =" << *p4 << std::endl;
        } // auto-delete p4

        ldl::SharedPointer<int> p5 = ldl::PoolAllocator<int>::New(99);
        std::cout << "p5 =" << p5 << std::endl;
        std::cout << "*p5 =" << *p5 << std::endl;

        //---------------

        // see if we can use it as an allocator for vectors

        typedef std::vector<int, ldl::PoolAllocator<int> > PoolVector;
        PoolVector v = PoolVector(ldl::PoolAllocator<int>());

        v.resize(10);



        std::cout << "done" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "exception in pool_allocator_test: " << ex.what() << std::endl;
    }
}


//-------------------------------------------

void promise_thread(ldl::Promise<int>* prom, int val ) {
    c11::this_thread::sleep_for(std::chrono::seconds(2));
    prom->set_value(val);
}

void future_test()
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
        c11::thread th(c11::bind(promise_thread, &prom,1));

        int val = fut.get();

        std::cout << "val = " << val << std::endl;

        th.join();

        //repeat
        ldl::Promise<int> prom2(&state);

        ldl::Future<int> fut2;
        fut2.swap(prom2.get_future()); // move assignment

        c11::thread th2(c11::bind(promise_thread, &prom2,2));

        int val2 = fut2.get();

        std::cout << "val2 = " << val2 << std::endl;

        th2.join();
    }
    catch (const std::exception& ex) {
        std::cout << "exception in future-test: " << ex.what() << std::endl;
    }
}

//-------------------------------------------
int main() {
    shared_ptr_test();
    pool_allocator_test();
    future_test();
    return 0;
}