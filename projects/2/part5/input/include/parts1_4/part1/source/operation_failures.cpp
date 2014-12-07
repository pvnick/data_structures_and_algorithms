#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Various operations fail and indicate as such with their return values" ) {
    GIVEN( "An HashMapOpenAddressing map" ) {
        cop3530::HashMapOpenAddressing map(1000);
        char value;
        REQUIRE(map.remove(5, value) == false);
        REQUIRE(map.search(5, value) == false);

        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(i, i + 1);
            
            THEN("Attempting to insert returns false") {
                REQUIRE(map.insert(5000, 5) == false);
            }
            AND_THEN("Attempting to remove a key that's not in the map returns false") {
                REQUIRE(map.remove(5000, value) == false);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns false") {
                REQUIRE(map.search(5000, value) == false);
            }
        }
    }
}

