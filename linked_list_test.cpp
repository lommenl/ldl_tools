#include "boost/test/unit_test.hpp"

#include "linked_list.h"
#include "pooled_new.h"

#include <iostream>

struct bar : public ldl::Linkable<bar>, ldl::PooledNew<bar> {
    int x;
};

BOOST_AUTO_TEST_CASE( linked_list_test )
{
    try {

        std::cout << "============================" << std::endl;
        std::cout << "Starting linked_list_test" << std::endl;
        std::cout << "============================" << std::endl;

        // x = 4 bytes, next_ptr_ = 8 bytes + 4 bytes of alignment padding
        BOOST_CHECK_EQUAL(sizeof(bar), 16);

        bar::SetPoolGrowthStep(10);

        ldl::LinkedList<bar> blist;

        BOOST_CHECK_EQUAL(blist.size(), 0);
        BOOST_CHECK_EQUAL(blist.empty(), true);

        bar x1;
        x1.x = 10;
        blist.push_front(x1);
        x1.x = 11;
        blist.push_front(x1);
        x1.x = 12;
        blist.push_front(x1);
        x1.x = 13;
        blist.push_front(x1);
        x1.x = 14;
        blist.push_front(x1);

        BOOST_CHECK_EQUAL(blist.size(), 5);
        BOOST_CHECK_EQUAL(blist.empty(), false);

        int chk = 14;
        for (ldl::LinkedList<bar>::const_iterator it = blist.cbegin(); it != blist.cend(); ++it) {
            BOOST_CHECK_EQUAL(it->x, chk--);
        }

        BOOST_CHECK_EQUAL(blist.front().x, 14);

        blist.pop_front();

        BOOST_CHECK_EQUAL(blist.front().x, 13);

        bar* p1 = new bar;
        p1->x = 15;
        blist.push_front(p1);

        BOOST_CHECK_EQUAL(blist.front().x, 15);

        blist.pop_front();

        BOOST_CHECK_EQUAL(blist.front().x, 13);

        std::cout << "done" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cout << "exception in linked_list_test: " << ex.what() << std::endl;
    }

}
