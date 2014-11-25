#include "catch.hpp"
#include <sstream>
#include "unit_tests.h"

SCENARIO("List removal" ) {
    GIVEN( "An integer list with 1000 items" ) {
        INSTANTIATE_LIST(int, l);
        
        REQUIRE_THROWS_AS( l.remove(0), std::out_of_range );
        REQUIRE_THROWS_AS( l.pop_front(), std::out_of_range );
        REQUIRE_THROWS_AS( l.pop_back(), std::out_of_range );

        for (int i = 0; i != 1000; ++i)
            l.insert(i, i);

        REQUIRE( l.size() == 1000 );

        WHEN( "An item is removed" ) {
            int ret_val = l.remove(42);
            THEN( "The size decreases by 1" ) {
                REQUIRE( l.size() == 999 );
            }
            AND_THEN( "The returned value has the expected value" )  {
                REQUIRE( ret_val == 42 );
            }
        }

        WHEN( "An attempt is made to remove an item from out of bounds" ) {
            THEN( "The list throws an out of range error" ) {
                REQUIRE_THROWS_AS( l.remove(1000), std::out_of_range );
                REQUIRE_THROWS_AS( l.remove(2000), std::out_of_range );
            }
        }
        
        WHEN( "An item is popped from the back" ) {
            int ret_val = l.pop_back();
            THEN( "The list size decreases by 1" ) {
                REQUIRE( l.size() == 999 );
            }
            AND_THEN( "The returned value has the expected value" )  {
                REQUIRE( ret_val == 999 );
            }
        }

        WHEN( "An item is popped from the front" ) {
            int ret_val = l.pop_front();
            THEN( "The list size decreases by 1" ) {
                REQUIRE( l.size() == 999 );
            }
            AND_THEN( "The returned value has the expected value" )  {
                REQUIRE( ret_val == 0 );
            }
        }
    }
}
