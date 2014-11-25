#include "catch.hpp"
#include <sstream>
#include "unit_tests.h"

SCENARIO("List item random access" ) {
    GIVEN( "An integer list that has been grown and shrunk acutely" ) {
        LIST<int> l;
        
        for (int i = 0; i != 1000; ++i) {
            l.push_back(1337);
            l.insert(1337, l.size() / 2);
            l.push_front(1337);
        }
        l.insert(-1, 42);
        while (l.size() > 100) {
            l.pop_back();
        }

        WHEN( "An item is retrieved via item_at()" ) {
            int item = l.item_at(42);
            THEN( "The item has the expected value" ) {
                REQUIRE( item == -1 );
            }
        }

        WHEN( "An out of bounds position is passed to item_at()" ) {
            THEN( "The list throws an out-of-range error" ) {
                REQUIRE_THROWS_AS(l.item_at(4000), std::out_of_range );
            }
        }
        WHEN( "An out of bounds position is passed to operator[]" ) {
            THEN( "The list throws an out-of-range error" ) {
                REQUIRE_THROWS_AS(l.operator[](4000), std::out_of_range );
            }
        }
        WHEN( "An out of bounds position is passed to const operator[]" ) {
            THEN( "The list throws an out-of-range error" ) {
                REQUIRE_THROWS_AS((const int&)l.operator[](4000), std::out_of_range );
            }
        }

        WHEN( "An item is retrieved via operator[]" ) {
            int& item = l[42];
            THEN( "The item has the expected value" ) {
                REQUIRE( item == -1 );
            }
            AND_WHEN( "We try to change the value of the item" ) {
                l[42] = 5;
                THEN( "Both our local item and the item stored in the list change" ) {
                    REQUIRE( item == 5 );
                    REQUIRE( l.item_at(42) == 5 );
                }
            }
        }
    }
}