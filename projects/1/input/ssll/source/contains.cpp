#include "catch.hpp"
#include <sstream>
#include "unit_tests.h"

SCENARIO("List item searching" ) {
    GIVEN( "An integer list that has been grown and shrunk acutely" ) {
        LIST<int> l;
        
        for (int i = 0; i != 1000; ++i) {
            l.push_back(i);
            l.insert(i, l.size() / 2);
            l.push_front(i);
        }
        while (l.size() > 100) {
            l.pop_back();
        }

        REQUIRE( ! l.contains(-1, [](const int& a, const int& b) -> bool { return a == b; }));
        WHEN("An item is inserted") {
            l.insert(-1, 42);
            THEN("The list contains the item") {
                REQUIRE(l.contains(-1, [](const int& a, const int& b) -> bool { return a == b; }));
            }
            AND_WHEN("The list is cleared") {
                l.clear();
                THEN("The list no longer contains the item") {
                    REQUIRE( ! l.contains(-1, [](const int& a, const int& b) -> bool { return a == b; }));
                }
            }
        }
    }
}