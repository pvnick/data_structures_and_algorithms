#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Various operations fail and indicate as such with their return values" ) {
    GIVEN( "An RBST map with integer keys" ) {
        cop3530::RBST<int, int> map(1000);
        int value;
        REQUIRE(map.remove(5, value) == 0);
        REQUIRE(map.search(5, value) == 0);

        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != 1000; ++i)
                map.insert(i, i + 1);
            
            THEN("Attempting to insert returns a negative value") {
                REQUIRE(map.insert(1001, 5) < 0);
            }
            AND_THEN("Attempting to remove a key that's not in the map returns a negative value") {
                REQUIRE(map.remove(1001, value) < 0);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(1001, value) < 0);
            }
        }
    }

    GIVEN( "An RBST map with double keys" ) {
        cop3530::RBST<double, int> map(1000);
        int value;
        REQUIRE(map.remove(5, value) == 0);
        REQUIRE(map.search(5, value) == 0);

        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != 1000; ++i)
                map.insert(i, i + 1);
            
            THEN("Attempting to insert returns a negative value") {
                REQUIRE(map.insert(1001, 5) < 0);
            }
            AND_THEN("Attempting to remove a key that's not in the map returns a negative value") {
                REQUIRE(map.remove(1001, value) < 0);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(1001, value) < 0);
            }
        }
    }

    GIVEN( "An RBST map with string keys" ) {
        cop3530::RBST<std::string, int> map(1000);
        int value;
        REQUIRE(map.remove(std::to_string(5), value) == 0);
        REQUIRE(map.search(std::to_string(5), value) == 0);

        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != 1000; ++i) {
                std::string str = std::to_string(i);
                map.insert(str.c_str(), i + 1);
            }
            THEN("Attempting to insert returns a negative value") {
                REQUIRE(map.insert(std::to_string(1001), 5) < 0);
            }
            AND_THEN("Attempting to remove a key that's not in the map returns a negative value") {
                REQUIRE(map.remove(std::to_string(1001), value) < 0);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(std::to_string(1001), value) < 0);
            }
        }
    }

    GIVEN( "An RBST map with const char* keys" ) {
        cop3530::RBST<std::string, int> map(1000);
        int value;
        REQUIRE(map.remove(std::to_string(5).c_str(), value) == 0);
        REQUIRE(map.search(std::to_string(5).c_str(), value) == 0);

        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != 1000; ++i) {
                std::string str = std::to_string(i);
                map.insert(str.c_str(), i + 1);
            }
            THEN("Attempting to insert returns a negative value") {
                REQUIRE(map.insert(std::to_string(1001).c_str(), 5) < 0);
            }
            AND_THEN("Attempting to remove a key that's not in the map returns a negative value") {
                REQUIRE(map.remove(std::to_string(1001).c_str(), value) < 0);
            }
            AND_THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(std::to_string(1001).c_str(), value) < 0);
            }
        }
    }
}

