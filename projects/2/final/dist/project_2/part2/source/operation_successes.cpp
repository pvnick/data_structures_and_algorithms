#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Certain basic operations should always succeed, with linear probing" ) {
    GIVEN( "A HashMapBuckets map with integer keys" ) {
        cop3530::HashMapBuckets map(1000);
        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(i, 'a' + (i % 26));
            map.clear();
            REQUIRE(map.size() == 0);
            REQUIRE(map.is_empty() == true);
            for (size_t i = 0; i != 500; ++i)
                map.insert(i, 'a' + (i % 26));
            
            THEN("The map reports itself as non-empty") {
                REQUIRE(map.size() == 500);
                REQUIRE( ! map.is_empty());
            }
            AND_THEN("Searching for a key known to be in the map should return its expected value") {
                char value;
                REQUIRE(map.search(50, value) == true);
                REQUIRE(value == 'a' + (50 % 26));

                REQUIRE(map.search(0, value) == true);
                REQUIRE(value == 'a');

                REQUIRE(map.search(499, value) == true);
                REQUIRE(value == 'a' + (499 % 26));
            }
            AND_THEN("Printing the map produces a realistic picture of its structure") {
                std::ostringstream oss;
                map.print(oss);
                std::string map_layout = oss.str();
                size_t num_empty_slots = 0;
                //check that the number of empty slots reported matches the calculated value
                for (int i = 0; i != map_layout.size(); ++i)
                    if (map_layout[i] == '-')
                        ++num_empty_slots;
                /*
                    Since Load factor = occupied buckets / capacity, we can get the number of unoccupied buckets as
                     capacity * (1 - load). This should equal the number of hyphens in the print
                */
                size_t unoccupied_buckets = std::ceil(map.capacity() * (1 - map.load()));
                REQUIRE(num_empty_slots == unoccupied_buckets);
            }
            AND_THEN("Removing a key known to be in the map should return its expected value") {
                char value;
                REQUIRE(map.remove(50, value) == true);
                REQUIRE(value == (char)('a' + (50 % 26)));

                REQUIRE(map.remove(0, value) == true);
                REQUIRE(value == 'a');

                REQUIRE(map.remove(499, value) == true);
                REQUIRE(value == (char)('a' + (499 % 26)));
                WHEN("The removed keys are searched for...") {
                    THEN("...via the search method, they are no longer found") {
                        char value;
                        REQUIRE(map.search(50, value) == false);
                        REQUIRE(map.search(0, value) == false);
                        REQUIRE(map.search(499, value) == false);
                    }
                    AND_THEN("...via the remove method, they are no longer found") {
                        char value;
                        REQUIRE(map.remove(50, value) == false);
                        REQUIRE(map.remove(0, value) == false);
                        REQUIRE(map.remove(499, value) == false);
                    }
                }
            }
        }
    }
}

