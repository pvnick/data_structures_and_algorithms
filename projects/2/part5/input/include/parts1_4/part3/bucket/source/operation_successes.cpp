#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Certain basic operations should always succeed, with linear probing" ) {
    GIVEN( "A HashMapBucketsGeneric map with integer keys" ) {
        cop3530::HashMapBucketsGeneric<int, int> map(1000);
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
                REQUIRE(map.size() == 500);
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
                int value;
                REQUIRE(map.remove(50, value) > 0);
                REQUIRE(value == 51);

                REQUIRE(map.remove(0, value) > 0);
                REQUIRE(value == 1);

                REQUIRE(map.remove(499, value) > 0);
                REQUIRE(value == 500);
                WHEN("The removed keys are searched for...") {
                    THEN("...via the search method, they are no longer found") {
                        int value;
                        REQUIRE(map.search(50, value) < 0);
                        REQUIRE(map.search(0, value) < 0);
                        REQUIRE(map.search(499, value) < 0);
                    }
                    AND_THEN("...via the remove method, they are no longer found") {
                        int value;
                        REQUIRE(map.remove(50, value) < 0);
                        REQUIRE(map.remove(0, value) < 0);
                        REQUIRE(map.remove(499, value) < 0);
                    }
                }
            }
            AND_WHEN("A random item is removed from the map") {
                int removed_key = map.remove_random();

                THEN("The removed key should no longer be found in the map with via the search() function") {
                    int value;
                    REQUIRE(map.search(removed_key, value) < 0);
                }
                AND_THEN("The removed key should no longer be found in the map with via the remove() function") {
                    int value;
                    REQUIRE(map.remove(removed_key, value) < 0);
                }
            }
        }
    }

    GIVEN( "A HashMapBucketsGeneric map with double keys" ) {
        cop3530::HashMapBucketsGeneric<double, int> map(1000);
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
                REQUIRE(map.size() == 500);
                REQUIRE( ! map.is_empty());
            }
            AND_THEN("Searching for a key known to be in the map should return its expected value") {
                int value;
                REQUIRE(map.search(2.5, value) > 0);
                REQUIRE(value == 6);

                REQUIRE(map.search(0, value) > 0);
                REQUIRE(value == 1);
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
                int value;
                REQUIRE(map.remove(2.5, value) > 0);
                REQUIRE(value == 6);

                REQUIRE(map.remove(0, value) > 0);
                REQUIRE(value == 1);

                WHEN("The removed keys are searched for...") {
                    THEN("...via the search method, they are no longer found") {
                        int value;
                        REQUIRE(map.search(2.5, value) < 0);
                        REQUIRE(map.search(0, value) < 0);
                    }
                    AND_THEN("...via the remove method, they are no longer found") {
                        int value;
                        REQUIRE(map.remove(2.5, value) < 0);
                        REQUIRE(map.remove(0, value) < 0);
                    }
                }
            }
            AND_WHEN("A random item is removed from the map") {
                double removed_key = map.remove_random();

                THEN("The removed key should no longer be found in the map with via the search() function") {
                    int value;
                    REQUIRE(map.search(removed_key, value) < 0);
                }
                AND_THEN("The removed key should no longer be found in the map with via the remove() function") {
                    int value;
                    REQUIRE(map.remove(removed_key, value) < 0);
                }
            }
        }
    }

    GIVEN( "A HashMapBucketsGeneric map with string keys" ) {
        cop3530::HashMapBucketsGeneric<std::string, int> map(1000);
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
                REQUIRE(map.size() == 500);
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
                int value;
                REQUIRE(map.remove("50", value) > 0);
                REQUIRE(value == 51);

                REQUIRE(map.remove("0", value) > 0);
                REQUIRE(value == 1);

                REQUIRE(map.remove("499", value) > 0);
                REQUIRE(value == 500);
                WHEN("The removed keys are searched for...") {
                    THEN("...via the search method, they are no longer found") {
                        int value;
                        REQUIRE(map.search("50", value) < 0);
                        REQUIRE(map.search("0", value) < 0);
                        REQUIRE(map.search("499", value) < 0);
                    }
                    AND_THEN("...via the remove method, they are no longer found") {
                        int value;
                        REQUIRE(map.remove("50", value) < 0);
                        REQUIRE(map.remove("0", value) < 0);
                        REQUIRE(map.remove("499", value) < 0);
                    }
                }
            }
            AND_WHEN("A random item is removed from the map") {
                std::string removed_key = map.remove_random();

                THEN("The removed key should no longer be found in the map with via the search() function") {
                    int value;
                    REQUIRE(map.search(removed_key, value) < 0);
                }
                AND_THEN("The removed key should no longer be found in the map with via the remove() function") {
                    int value;
                    REQUIRE(map.remove(removed_key, value) < 0);
                }
            }
        }
    }

    GIVEN( "A HashMapBucketsGeneric map with const char* keys" ) {
        cop3530::HashMapBucketsGeneric<const char*, int> map(1000);
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
                REQUIRE(map.size() == 500);
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
                int value;
                REQUIRE(map.remove("50", value) > 0);
                REQUIRE(value == 51);

                REQUIRE(map.remove("0", value) > 0);
                REQUIRE(value == 1);

                REQUIRE(map.remove("499", value) > 0);
                REQUIRE(value == 500);
                WHEN("The removed keys are searched for...") {
                    THEN("...via the search method, they are no longer found") {
                        int value;
                        REQUIRE(map.search("50", value) < 0);
                        REQUIRE(map.search("0", value) < 0);
                        REQUIRE(map.search("499", value) < 0);
                    }
                    AND_THEN("...via the remove method, they are no longer found") {
                        int value;
                        REQUIRE(map.remove("50", value) < 0);
                        REQUIRE(map.remove("0", value) < 0);
                        REQUIRE(map.remove("499", value) < 0);
                    }
                }
            }
            WHEN("A random item is removed from the map") {
                const char* removed_key = map.remove_random();
                THEN("The removed key should no longer be found in the map with via the search() function") {
                    int value;
                    REQUIRE(map.search(removed_key, value) < 0);
                }
                AND_THEN("The removed key should no longer be found in the map with via the remove() function") {
                    int value;
                    REQUIRE(map.remove(removed_key, value) < 0);
                }
            }
        }
    }
}

