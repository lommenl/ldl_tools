#include "boost/test/unit_test.hpp"

#include "linked_list.h"

#include "pooled_new.h"

#include "static_pool_list.h"


struct bar : public ldl::Linkable, public ldl::PooledNew<bar> {
    int x;
};

BOOST_AUTO_TEST_SUITE(LINKED_LIST)

BOOST_AUTO_TEST_CASE( linked_list_test )
{
    BOOST_TEST_MESSAGE("Starting linked_list_test");
    try {

        // let all pools grow as needed
        ldl::StaticPoolList::SetPoolGrowthStep(0, 10);

        // padded to 64-bit alignment
        size_t sz = (sizeof(int) + sizeof(ldl::Linkable)+7)&~7;
        BOOST_CHECK_EQUAL(sizeof(bar), sz);

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

    }
    catch (const std::exception& ex) {
        BOOST_TEST_MESSAGE("exception in linked_list_test: " << ex.what());
    }

}
BOOST_AUTO_TEST_SUITE_END()

