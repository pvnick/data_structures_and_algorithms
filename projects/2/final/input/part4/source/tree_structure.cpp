#include "unit_tests.h"
#include "catch/catch.hpp"

/*
    Note about the following method:
    We pretty-print the tree to a file and filter out everything but the keys.
    If the BST tree has the proper structure, those keys will exist in the file naturally sorted.
    We check this by piping the keys through uniq and sort to eliminate duplicates and sort
    the keys to how we would expect. If the result of the second operation does not change the 
    contents of the file, the tree has proper structure
*/

SCENARIO( "Valid BST tree structure is maintained" ) {
    /*
        numeric type keys, notice the -n flag in the sort command when validating structure
    */
    GIVEN( "An RBST map with integer keys" ) {
        cop3530::RBST<int, int> map(1000);

        REQUIRE(map.is_empty());
        REQUIRE(map.size() == 0);
        REQUIRE(map.load() == 0.0);
        std::ostringstream oss;

        WHEN("The tree is filled to half-capacity") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(i, i + 1);

            THEN("The load() method reports 50% load factor") {
                REQUIRE(map.load() == 0.5);
            }
            AND_THEN("The BST tree structure is maintained") {
                std::string tree_out_file = cop3530::unit_test_utils::get_tmp_filename();
                std::ofstream out(tree_out_file);
                map.print(out);
                out.close();
                //get the keys from the pretty-print tree structure and compare their natural order to ascending numerical order
                std::string unsorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | tr '\n' ',' "));
                std::string sorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | uniq | sort -rn | tr '\n' ','"));
                REQUIRE(unsorted_keys == sorted_keys);
                cop3530::unit_test_utils::delete_file(tree_out_file);
            }
            AND_WHEN("The tree is filled to full-capacity") {
                for (size_t i = 500; i != 1000; ++i)
                    map.insert(i, i + 1);

                THEN("The load() method reports 100% load factor") {
                    REQUIRE(map.load() == 1);
                }

                AND_WHEN("Half the items are randomly deleted") {
                    for (size_t i = 0; i != 250; ++i) {
                        int dummy;
                        map.remove(i, dummy);
                    }
                    for (size_t i = 0; i != 250; ++i)
                        map.remove_random();

                    THEN("The load() method reports 50% load factor") {
                        REQUIRE(map.load() == 0.5);
                    }
                    AND_THEN("The BST tree structure is maintained") {
                        std::string tree_out_file = cop3530::unit_test_utils::get_tmp_filename();
                        std::ofstream out(tree_out_file);
                        map.print(out);
                        out.close();
                        //get the keys from the pretty-print tree structure and compare their natural order to ascending numerical order
                        std::string unsorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | tr '\n' ',' "));
                        std::string sorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | uniq | sort -rn | tr '\n' ','"));
                        REQUIRE(unsorted_keys == sorted_keys);
                        cop3530::unit_test_utils::delete_file(tree_out_file);
                    }
                }
            }
        }
    }

    GIVEN( "An RBST map with double keys" ) {
        cop3530::RBST<double, int> map(1000);

        REQUIRE(map.is_empty());
        REQUIRE(map.size() == 0);
        REQUIRE(map.load() == 0.0);
        std::ostringstream oss;

        WHEN("The tree is filled to half-capacity") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(static_cast<double>(i) / 2.0, i + 1);

            THEN("The load() method reports 50% load factor") {
                REQUIRE(map.load() == 0.5);
            }
            AND_THEN("The BST tree structure is maintained") {
                std::string tree_out_file = cop3530::unit_test_utils::get_tmp_filename();
                std::ofstream out(tree_out_file);
                map.print(out);
                out.close();
                //get the keys from the pretty-print tree structure and compare their natural order to ascending numerical order
                std::string tree_order_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9\\.]+$' | tr '\n' ',' "));
                std::string sorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9\\.]+$' | uniq | sort -nr | tr '\n' ','"));
                REQUIRE(tree_order_keys == sorted_keys);
                cop3530::unit_test_utils::delete_file(tree_out_file);
            }
            AND_WHEN("The tree is filled to full-capacity") {
                for (size_t i = 500; i != 1000; ++i)
                    map.insert(static_cast<double>(i) / 2.0, i + 1);

                THEN("The load() method reports 100% load factor") {
                    REQUIRE(map.load() == 1);
                }

                AND_WHEN("Half the items are randomly deleted") {
                    for (size_t i = 0; i != 250; ++i) {
                        int dummy;
                        map.remove(static_cast<double>(i) / 2.0, dummy);
                    }
                    for (size_t i = 0; i != 250; ++i)
                        map.remove_random();

                    THEN("The load() method reports 50% load factor") {
                        REQUIRE(map.load() == 0.5);
                    }
                    AND_THEN("The BST tree structure is maintained") {
                        std::string tree_out_file = cop3530::unit_test_utils::get_tmp_filename();
                        std::ofstream out(tree_out_file);
                        map.print(out);
                        out.close();
                        //get the keys from the pretty-print tree structure and compare their natural order to ascending numerical order
                        std::string tree_order_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9\\.]+$' | tr '\n' ',' "));
                        std::string sorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9\\.]+$' | uniq | sort -nr | tr '\n' ','"));
                        REQUIRE(tree_order_keys == sorted_keys);
                        cop3530::unit_test_utils::delete_file(tree_out_file);
                    }
                }
            }
        }
    }

    /*
        string type keys, notice the absence of the -n flag in the sort command when validating structure
    */
    GIVEN( "An RBST map with string keys" ) {
        cop3530::RBST<std::string, int> map(1000);

        REQUIRE(map.is_empty());
        REQUIRE(map.size() == 0);
        REQUIRE(map.load() == 0.0);
        std::ostringstream oss;

        WHEN("The tree is filled to half-capacity") {
            for (size_t i = 0; i != 500; ++i)
                map.insert(std::to_string(i), i + 1);

            THEN("The load() method reports 50% load factor") {
                REQUIRE(map.load() == 0.5);
            }
            AND_THEN("The BST tree structure is maintained") {
                std::string tree_out_file = cop3530::unit_test_utils::get_tmp_filename();
                std::ofstream out(tree_out_file);
                map.print(out);
                out.close();
                //get the keys from the pretty-print tree structure and compare their natural order to ascending numerical order
                std::string tree_order_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | tr '\n' ',' "));
                std::string sorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | uniq | sort -r | tr '\n' ','"));
                REQUIRE(tree_order_keys == sorted_keys);
                cop3530::unit_test_utils::delete_file(tree_out_file);
            }
            AND_WHEN("The tree is filled to full-capacity") {
                for (size_t i = 500; i != 1000; ++i)
                    map.insert(std::to_string(i), i + 1);

                THEN("The load() method reports 100% load factor") {
                    REQUIRE(map.load() == 1);
                }

                AND_WHEN("Half the items are randomly deleted") {
                    for (size_t i = 0; i != 250; ++i) {
                        int dummy;
                        map.remove(std::to_string(i), dummy);
                    }
                    for (size_t i = 0; i != 250; ++i)
                        map.remove_random();

                    THEN("The load() method reports 50% load factor") {
                        REQUIRE(map.load() == 0.5);
                    }
                    AND_THEN("The BST tree structure is maintained") {
                        std::string tree_out_file = cop3530::unit_test_utils::get_tmp_filename();
                        std::ofstream out(tree_out_file);
                        map.print(out);
                        out.close();
                        //get the keys from the pretty-print tree structure and compare their natural order to ascending numerical order
                        std::string tree_order_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | tr '\n' ',' "));
                        std::string sorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | uniq | sort -r | tr '\n' ','"));
                        REQUIRE(tree_order_keys == sorted_keys);
                        cop3530::unit_test_utils::delete_file(tree_out_file);
                    }
                }
            }
        }
    }

    GIVEN( "An RBST map with const char* keys" ) {
        cop3530::RBST<const char*, int> map(1000);

        REQUIRE(map.is_empty());
        REQUIRE(map.size() == 0);
        REQUIRE(map.load() == 0.0);
        std::ostringstream oss;

        WHEN("The tree is filled to half-capacity") {
            for (size_t i = 0; i != 500; ++i) {
                std::string str = std::to_string(i);
                map.insert(str.c_str(), i + 1);
            }
            THEN("The load() method reports 50% load factor") {
                REQUIRE(map.load() == 0.5);
            }
            AND_THEN("The BST tree structure is maintained") {
                std::string tree_out_file = cop3530::unit_test_utils::get_tmp_filename();
                std::ofstream out(tree_out_file);
                map.print(out);
                out.close();
                //get the keys from the pretty-print tree structure and compare their natural order to ascending numerical order
                std::string tree_order_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | tr '\n' ',' "));
                std::string sorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | uniq | sort -r | tr '\n' ','"));
                REQUIRE(tree_order_keys == sorted_keys);
                cop3530::unit_test_utils::delete_file(tree_out_file);
            }
            AND_WHEN("The tree is filled to full-capacity") {
                for (size_t i = 500; i != 1000; ++i) {
                    std::string str = std::to_string(i);
                    map.insert(str.c_str(), i + 1);
                }
                THEN("The load() method reports 100% load factor") {
                    REQUIRE(map.load() == 1);
                }

                AND_WHEN("Half the items are randomly deleted") {
                    for (size_t i = 0; i != 250; ++i) {
                        std::string str = std::to_string(i);
                        int dummy;
                        map.remove(str.c_str(), dummy);
                    }
                    for (size_t i = 0; i != 250; ++i)
                        map.remove_random();

                    THEN("The load() method reports 50% load factor") {
                        REQUIRE(map.load() == 0.5);
                    }
                    AND_THEN("The BST tree structure is maintained") {
                        std::string tree_out_file = cop3530::unit_test_utils::get_tmp_filename();
                        std::ofstream out(tree_out_file);
                        map.print(out);
                        out.close();
                        //get the keys from the pretty-print tree structure and compare their natural order to ascending numerical order
                        std::string tree_order_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | tr '\n' ',' "));
                        std::string sorted_keys = cop3530::unit_test_utils::shell_cmd(std::string("cat ") + tree_out_file + std::string(" | cut -d'[' -f2 | cut -d']' -f1 | grep -E '^[0-9]+$' | uniq | sort -r | tr '\n' ','"));
                        REQUIRE(tree_order_keys == sorted_keys);
                        cop3530::unit_test_utils::delete_file(tree_out_file);
                    }
                }
            }
        }
    }
}

