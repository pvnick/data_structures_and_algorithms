#include "unit_tests.h"
#include "catch/catch.hpp"

SCENARIO( "Certain basic operations should always succeed" ) {
    GIVEN( "An RBST map with integer keys" ) {
        typedef int key_type;
        cop3530::RBST<key_type, int> map(1000);
        REQUIRE(map.capacity() == 1000);
        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(i, i + 1);
            map.clear();
            REQUIRE(map.capacity() == 1000);
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
                key_type removed_key = map.remove_random();

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

    GIVEN( "An RBST map with double keys" ) {
        typedef double key_type;
        cop3530::RBST<key_type, int> map(1000);
        REQUIRE(map.capacity() == 1000);
        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(static_cast<double>(i) / 2.0, i + 1);
            map.clear();
            REQUIRE(map.capacity() == 1000);
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
                key_type removed_key = map.remove_random();

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

    GIVEN( "An RBST map with string keys" ) {
        typedef std::string key_type;
        cop3530::RBST<key_type, int> map(1000);
        REQUIRE(map.capacity() == 1000);
        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(std::to_string(i), i + 1);
            map.clear();
            REQUIRE(map.capacity() == 1000);
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
                key_type removed_key = map.remove_random();

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

    GIVEN( "An RBST map with const char* keys" ) {
        typedef const char* key_type;
        cop3530::RBST<key_type, int> map(1000);
        REQUIRE(map.capacity() == 1000);
        REQUIRE(map.size() == 0);
        REQUIRE(map.is_empty() == true);

        WHEN("The map is filled halfway, cleared, then filled halfway again") {
            for (size_t i = 0; i != 500; ++i) {
                std::string str = std::to_string(i);
                map.insert(str.c_str(), i + 1);
            }
            map.clear();
            REQUIRE(map.capacity() == 1000);
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
                key_type removed_key = map.remove_random();

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

