#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Various operations fail and indicate as such with their return values" ) {
    GIVEN( "An HashMapBucketsGeneric map with integer keys" ) {
        cop3530::HashMapBuckets map(1000);
        char value;
        REQUIRE(map.remove(5, value) == false);
        REQUIRE(map.search(5, value) == false);

        WHEN("The map is filled with a bunch of items, cleared, then filled again") {
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(i, i + 1);
            map.clear();
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(i, i + 1);
            
            THEN("Attempting to remove a key that's not in the map returns false") {
                REQUIRE(map.remove(5000, value) == false);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns false") {
                REQUIRE(map.search(5000, value) == false);
            }
        }
    }
}

