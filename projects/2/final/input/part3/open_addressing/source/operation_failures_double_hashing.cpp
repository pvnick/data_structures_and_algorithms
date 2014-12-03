#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Various operations fail and indicate as such with their return values, double hashing" ) {
    GIVEN( "A HashMapOpenAddressingGeneric map with integer keys" ) {
        cop3530::HashMapOpenAddressingGeneric<int, 
                                              int, 
                                              cop3530::hash_utils::functors::map_capacity_planner,
                                              cop3530::hash_utils::functors::primary_hashes::hash_basic,
                                              cop3530::hash_utils::functors::secondary_hashes::hash_double> map(1000);
        int value;
        REQUIRE(map.search(5, value) == -1);

        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != 1024; ++i)
                map.insert(i, i + 1);
            
            THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(2000, value) < 0);
            }
        }
    }

    GIVEN( "A HashMapOpenAddressingGeneric map with double keys" ) {
        cop3530::HashMapOpenAddressingGeneric<double, 
                                              int, 
                                              cop3530::hash_utils::functors::map_capacity_planner,
                                              cop3530::hash_utils::functors::primary_hashes::hash_basic,
                                              cop3530::hash_utils::functors::secondary_hashes::hash_double> map(1000);        int value;
        REQUIRE(map.search(5, value) == -1);

        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != 1024; ++i)
                map.insert(i, i + 1);
            
            THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(2000, value) < 0);
            }
        }
    }

    GIVEN( "A HashMapOpenAddressingGeneric map with string keys" ) {
        cop3530::HashMapOpenAddressingGeneric<std::string, 
                                              int, 
                                              cop3530::hash_utils::functors::map_capacity_planner,
                                              cop3530::hash_utils::functors::primary_hashes::hash_basic,
                                              cop3530::hash_utils::functors::secondary_hashes::hash_double> map(1000);
        int value;
        REQUIRE(map.search(std::to_string(5), value) == -1);

        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != 1024; ++i) {
                std::string str = std::to_string(i);
                map.insert(str, i + 1);
            }
            THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(std::to_string(2000), value) < 0);
            }
        }
    }

    GIVEN( "A HashMapOpenAddressingGeneric map with const char* keys" ) {
        cop3530::HashMapOpenAddressingGeneric<const char*, 
                                              int, 
                                              cop3530::hash_utils::functors::map_capacity_planner,
                                              cop3530::hash_utils::functors::primary_hashes::hash_basic,
                                              cop3530::hash_utils::functors::secondary_hashes::hash_double> map(1000);
        int value;
        REQUIRE(map.search(std::to_string(5).c_str(), value) == -1);
        WHEN("The map is filled to capacity") {
            for (size_t i = 0; i != 1024; ++i) {
                std::string str = std::to_string(i);
                map.insert(str.c_str(), i + 1);
            }
            THEN("Attempting to search for a key that's not in the map returns a negative value") {
                REQUIRE(map.search(std::to_string(2000).c_str(), value) < 0);
            }
        }
    }
}

