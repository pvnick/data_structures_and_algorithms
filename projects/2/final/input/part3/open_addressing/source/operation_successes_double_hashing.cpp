#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Certain basic operations should always succeed, with double hashing" ) {
    GIVEN( "A HashMapOpenAddressingGeneric map with integer keys" ) {
        typedef int key_type;
        cop3530::HashMapOpenAddressingGeneric<key_type, 
                                              int, 
                                              cop3530::hash_utils::functors::map_capacity_planner,
                                              cop3530::hash_utils::functors::primary_hashes::hash_basic,
                                              cop3530::hash_utils::functors::secondary_hashes::hash_double> map(1000);

        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(i, i + 1);
            map.clear();
            REQUIRE(map.size() == 0);
            REQUIRE(map.is_empty() == true);
            for (size_t i = 0; i != 500; ++i)
                map.insert(i, i + 1);
            
            THEN("The map reports itself as non-empty") {
                REQUIRE( ! map.is_empty());
            }
            AND_THEN("Searching for a key known to be in the map should return its expected value") {
                int value;
                REQUIRE(map.search(50, value) > 0);
                REQUIRE(value == 51);

                REQUIRE(map.search(0, value) > 0);
                REQUIRE(value == 1);

                REQUIRE(map.search(499, value) > 0);
                REQUIRE(value == 500);
            }
        }
    }

    GIVEN( "A HashMapOpenAddressingGeneric map with double keys" ) {
        typedef double key_type;
        cop3530::HashMapOpenAddressingGeneric<key_type, 
                                              int, 
                                              cop3530::hash_utils::functors::map_capacity_planner,
                                              cop3530::hash_utils::functors::primary_hashes::hash_basic,
                                              cop3530::hash_utils::functors::secondary_hashes::hash_double> map(1000);
        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(static_cast<double>(i) / 2.0, i + 1);
            map.clear();
            REQUIRE(map.size() == 0);
            REQUIRE(map.is_empty() == true);
            for (size_t i = 0; i != 500; ++i)
                map.insert(static_cast<double>(i) / 2.0, i + 1);
            
            THEN("The map reports itself as non-empty") {
                REQUIRE( ! map.is_empty());
            }
            AND_THEN("Searching for a key known to be in the map should return its expected value") {
                int value;
                REQUIRE(map.search(2.5, value) > 0);
                REQUIRE(value == 6);

                REQUIRE(map.search(0, value) > 0);
                REQUIRE(value == 1);
            }
        }
    }

    GIVEN( "A HashMapOpenAddressingGeneric map with string keys" ) {
        typedef std::string key_type;
        cop3530::HashMapOpenAddressingGeneric<key_type, 
                                              int, 
                                              cop3530::hash_utils::functors::map_capacity_planner,
                                              cop3530::hash_utils::functors::primary_hashes::hash_basic,
                                              cop3530::hash_utils::functors::secondary_hashes::hash_double> map(1000);
        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(std::to_string(i), i + 1);
            map.clear();
            REQUIRE(map.size() == 0);
            REQUIRE(map.is_empty() == true);
            for (size_t i = 0; i != 500; ++i)
                map.insert(std::to_string(i), i + 1);
            
            THEN("The map reports itself as non-empty") {
                REQUIRE( ! map.is_empty());
            }
            AND_THEN("Searching for a key known to be in the map should return its expected value") {
                int value;
                REQUIRE(map.search("50", value) > 0);
                REQUIRE(value == 51);

                REQUIRE(map.search("0", value) > 0);
                REQUIRE(value == 1);

                REQUIRE(map.search("499", value) > 0);
                REQUIRE(value == 500);
            }
        }
    }

    GIVEN( "A HashMapOpenAddressingGeneric map with const char* keys" ) {
        typedef const char* key_type;
        cop3530::HashMapOpenAddressingGeneric<key_type, 
                                              int, 
                                              cop3530::hash_utils::functors::map_capacity_planner,
                                              cop3530::hash_utils::functors::primary_hashes::hash_basic,
                                              cop3530::hash_utils::functors::secondary_hashes::hash_double> map(1000);
        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i) {
                std::string str = std::to_string(i);
                map.insert(str.c_str(), i + 1);
            }
            map.clear();
            REQUIRE(map.size() == 0);
            REQUIRE(map.is_empty() == true);
            for (size_t i = 0; i != 500; ++i) {
                std::string str = std::to_string(i);
                map.insert(str.c_str(), i + 1);
            }
            
            THEN("The map reports itself as non-empty") {
                REQUIRE( ! map.is_empty());
            }
            AND_THEN("Searching for a key known to be in the map should return its expected value") {
                int value;
                REQUIRE(map.search("50", value) > 0);
                REQUIRE(value == 51);

                REQUIRE(map.search("0", value) > 0);
                REQUIRE(value == 1);

                REQUIRE(map.search("499", value) > 0);
                REQUIRE(value == 500);
            }
        }
    }
}

