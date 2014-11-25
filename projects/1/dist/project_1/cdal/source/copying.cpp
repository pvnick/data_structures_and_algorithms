#include "catch.hpp"
#include <sstream>
#include "unit_tests.h"

SCENARIO("List copying" ) {
    GIVEN( "An integer list that has been grown and shrunk acutely" ) {
        LIST<int> l;
        for (int i = 0; i != 1000; ++i) {
            l.push_back(i);
            l.insert(i, l.size() / 2);
            l.push_front(i);
        }
        l.insert(-1, 42);
        while (l.size() > 100) {
            l.pop_back();
        }

        WHEN( "The list is copied via copy constructor" ) {
            LIST<int> l2(l);
            int& item = l2[42];
            THEN( "The item has the expected value" ) {
                REQUIRE( item == -1 );
            }
            AND_WHEN( "We try to change the value of the item" ) {
                l2[42] = 5;
                THEN( "The value changes in the copy but not the original" ) {
                    REQUIRE( l2[42] == 5 );
                    REQUIRE( l[42] == -1 );
                }
            }
        }

        WHEN( "A second list has been grown and shrunk acutely" ) {
            LIST<int> l2;
            for (int i = 1000; i != 2000; ++i) {
                l2.push_back(i);
                l2.insert(i, l2.size() / 2);
                l2.push_front(i);
            }
            while (l2.size() > 100) {
                l2.pop_back();
            }
            AND_WHEN("The second list's copy assigner is invoked with the original list as the source") {
                l2 = l;
                THEN( "The two items contain the same contents" ) {
                    std::ostringstream l_contents;
                    l.print(l_contents);
                    std::ostringstream l2_contents;
                    l2.print(l2_contents);
                    REQUIRE( l_contents.str() == l2_contents.str() );
                }
                AND_WHEN( "We change the value of an item in the list copy" ) {
                    l2[42] = 5;
                    THEN( "The value changes in the copy but not the original" ) {
                        REQUIRE( l2[42] == 5 );
                        REQUIRE( l[42] == -1 );
                    }
                }
            }
        }
    }
}