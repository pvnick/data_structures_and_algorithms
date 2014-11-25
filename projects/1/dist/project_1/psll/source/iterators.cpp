#include "catch.hpp"
#include <sstream>
#include "unit_tests.h"

SCENARIO("List iterators" ) {
    GIVEN( "An empty integer list" ) {
        LIST<int> l;

        WHEN("A regular and const iterator object are instantiated")
        {
            LIST<int>::const_iterator const_iter = l.begin();
            LIST<int>::const_iterator const_iter_end = l.begin();
            LIST<int>::const_iterator iter = l.begin();
            LIST<int>::const_iterator iter_end = l.begin();
            THEN("The fresh iterators are equivalent to the end iterator") {
                REQUIRE(const_iter == const_iter_end);
                REQUIRE(iter == iter_end);
                REQUIRE_THROWS_AS( *const_iter, std::out_of_range );
                REQUIRE_THROWS_AS( *iter, std::out_of_range );            
            }
        }
        WHEN("The list is grown and shrunk acutely") {
            for (int i = 0; i != 1000; ++i) {
                l.push_back(i);
                l.insert(i, l.size() / 2);
                l.push_front(i);
            }
            while (l.size() > 10) {
                l.pop_front();
            }
            THEN("A regular or const iterator traversing the list should visit size() slots") {
                LIST<int>::const_iterator const_iter = l.begin();
                LIST<int>::const_iterator const_iter_end = l.end();
                LIST<int>::const_iterator iter = l.begin();
                LIST<int>::const_iterator iter_end = l.end();
                size_t iter_counter = 0;
                size_t const_iter_counter = 0;
                size_t sz = l.size();
                for (; iter != iter_end; ++iter, ++iter_counter);
                for (; iter != iter_end; ++iter, ++const_iter_counter);
                REQUIRE(iter_counter == sz);
            }
            AND_WHEN("The client regular iterator shoud be able to change the contents of the list") {
                LIST<int>::iterator iter = l.begin();
                LIST<int>::iterator iter2 = iter;
                LIST<int>::iterator iter_end = l.end();
                size_t slot = 0;
                for (; iter != iter_end; ++iter)
                    *iter = slot++;
                THEN("The iterator can be reassigned") {
                    iter = l.begin();
                    REQUIRE(iter != l.end());
                    REQUIRE(iter == iter2);
                    THEN("Post-increment returns the unincremented iterator") {
                        LIST<int>::iterator i_copy = iter;
                        REQUIRE(i_copy == iter++);
                        THEN("Post-increment incremented the original iterator") {
                            REQUIRE(*iter == 1);
                        }
                    }
                    THEN("Pre-increment returns the incremented iterator") {
                        LIST<int>::iterator i_copy = iter;
                        LIST<int>::iterator incremented = ++iter;
                        REQUIRE(i_copy != incremented);
                        i_copy++;
                        REQUIRE(i_copy == incremented);
                        REQUIRE(i_copy == iter);
                    }
                    AND_THEN("The reassigned iterator can re-read the list and verify its new contents") {
                        size_t slot = 0;
                        for (; iter != iter_end; ++iter) {
                            REQUIRE(*iter == slot++);
                        }
                        AND_THEN("The reassigned iterator, which is at the end of the list, cannot be incremented anymore") {
                            REQUIRE_THROWS_AS( ++iter, std::out_of_range );
                        }
                    }
                }
            }
        }
    }
}