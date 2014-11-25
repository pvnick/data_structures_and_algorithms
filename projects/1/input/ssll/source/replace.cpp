#include "catch.hpp"
#include <sstream>
#include "unit_tests.h"

SCENARIO("List item replacement" ) {
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

        WHEN( "An item is replaced" ) {
            size_t old_size = l.size();
            int new_item = 5;
            int old_item = l.replace(new_item, 42);
            THEN( "The replaced item, returned from replace(), has the expected value" ) {
                REQUIRE( old_item == -1 );
            }
            AND_THEN( "The new item is now stored in the list at the replacement position" ) {
                REQUIRE( l[42] == 5 );
            }
            AND_THEN( "The list size did not change" ) {
                REQUIRE( l.size() == old_size );
            }
        }

        WHEN( "An out of bounds position is passed to replace()" ) {
            THEN( "The list throws an out-of-range error" ) {
                REQUIRE_THROWS_AS(l.replace(7, l.size()), std::out_of_range );
                REQUIRE_THROWS_AS(l.replace(7, 5000), std::out_of_range );
            }
        }
    }
}