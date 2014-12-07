#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Various operations fail and indicate as such with their return values" ) {
    GIVEN( "An HashMapBucketsGeneric map with integer keys" ) {
        cop3530::HashMapBucketsGeneric<int, int> map(1000);
        int value;
        REQUIRE(map.remove(5, value) == -1);
        REQUIRE(map.search(5, value) == -1);

        WHEN("The map is filled with a bunch of items, cleared, then filled again") {
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(i, i + 1);
            map.clear();
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(i, i + 1);
            
            THEN("Attempting to remove a key that's not in the map returns a negative value") {
                REQUIRE(map.remove(5000, value) < 0);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(5000, value) < 0);
            }
        }
    }

    GIVEN( "An HashMapBucketsGeneric map with double keys" ) {
        cop3530::HashMapBucketsGeneric<double, int> map(1000);
        int value;
        REQUIRE(map.remove(5, value) == -1);
        REQUIRE(map.search(5, value) == -1);

        WHEN("The map is filled with a bunch of items, cleared, then filled again") {
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(static_cast<double>(i) / 2.0, i + 1);
            map.clear();
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(static_cast<double>(i) / 2.0, i + 1);

            THEN("Attempting to remove a key that's not in the map returns a negative value") {
                REQUIRE(map.remove(5000, value) < 0);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(5000, value) < 0);
            }
        }
    }

    GIVEN( "An HashMapBucketsGeneric map with string keys" ) {
        cop3530::HashMapBucketsGeneric<std::string, int> map(1000);
        int value;
        REQUIRE(map.remove(std::to_string(5), value) == -1);
        REQUIRE(map.search(std::to_string(5), value) == -1);

        WHEN("The map is filled with a bunch of items, cleared, then filled again") {
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(std::to_string(i), i + 1);
            map.clear();
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(std::to_string(i), i + 1);
            THEN("Attempting to remove a key that's not in the map returns a negative value") {
                REQUIRE(map.remove(std::to_string(5000), value) < 0);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(std::to_string(5000), value) < 0);
            }
        }
    }

    GIVEN( "An HashMapBucketsGeneric map with const char* keys" ) {
        cop3530::HashMapBucketsGeneric<const char*, int> map(1000);
        int value;
        REQUIRE(map.remove(std::to_string(5).c_str(), value) == -1);
        REQUIRE(map.search(std::to_string(5).c_str(), value) == -1);

        WHEN("The map is filled with a bunch of items, cleared, then filled again") {
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(std::to_string(i).c_str(), i + 1);
            map.clear();
            for (size_t i = 0; i != map.capacity(); ++i)
                map.insert(std::to_string(i).c_str(), i + 1);
            THEN("Attempting to remove a key that's not in the map returns a negative value") {
                REQUIRE(map.remove(std::to_string(5000).c_str(), value) < 0);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(std::to_string(5000).c_str(), value) < 0);
            }
        }
    }
}

