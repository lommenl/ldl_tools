#include <iostream>

#include "shared_pointer.h"

//#include "pool_allocator.h"

int main() {

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


        // todo:
        // SharedPointer(ptr,mx)
        // SharedPointer(ptr,del)
        // SharedPointer(ptr,del,mx)
        // operator=(SharedPointer<U,M>&)
        // reset(ptr,mx)
        // reset(ptr,del)
        // reset(ptr,del,mx)
        // Constructors and reset() with mutex
        // Constructors and reset() with Deleter and mutex
        // operator->()
        //----
        // swap(lhs,rhs)
        // comparison operators

        

        std::cout << "done" << std::endl;

    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }


#if 0
    // Pool with 2 elements
    PoolAllocator<int>::Pool(1).reserve(2);

    if (1) {
        // allocate 1st element
        boost::shared_ptr<int> sp1 = PoolSharedPtr<int>();

        std::cout << "p1 = " << sp1.get() << std::endl;
    } // free 1st element

    // allocate 2nd element
    int* p2 = PoolAllocator<int>::Pool(1).New();
    std::cout << "p2 = " << p2 << std::endl;

    // re-use 1st element
    int* p3 = PoolAllocator<int>::Pool(1).New();

    std::cout << "p3 = " << p3 << std::endl;

    PoolAllocator<int>::Pool(1).Delete(p2);
    PoolAllocator<int>::Pool(1).Delete(p3);

    //-----



    //-----
#endif //FOOXXX

    return 0;
}