#include <sstream>

SCENARIO("List insertion" ) {
    GIVEN( "An empty integer list" ) {
        INSTANTIATE_LIST(int, l);

        REQUIRE( l.size() == 0 );
        std::ostringstream contents;
        l.print(contents);
        REQUIRE( contents.str() == "<empty list>" );

        REQUIRE( l.size() == 0 );

        WHEN( "An item is inserted" ) {
            l.insert(1337, 0);
            THEN( "The size increases by 1" ) {
                REQUIRE( l.size() == 1 );
            }
            AND_THEN( "Printing the contents reflects the inserted item")  {
                std::ostringstream new_contents;
                l.print(new_contents);
                REQUIRE( new_contents.str() == "[1337]" );
            }
            AND_WHEN( "An item is inserted at the end of the list" ) {
                l.insert(42, 1);
                THEN( "The size increases by 1" ) {
                    REQUIRE( l.size() == 2 );
                }
                AND_THEN( "Printing the contents reflects the inserted item")  {
                    std::ostringstream new_contents;
                    l.print(new_contents);
                    REQUIRE( new_contents.str() == "[1337,42]" );
                }
                AND_WHEN( "An item is pushed back" ) {
                    l.push_back(100);
                    THEN( "The size increases by 1" ) {
                        REQUIRE( l.size() == 3 );
                    }
                    AND_THEN( "Printing the contents reflects the inserted item")  {
                        std::ostringstream new_contents;
                        l.print(new_contents);
                        REQUIRE( new_contents.str() == "[1337,42,100]" );
                    }
                }
                AND_WHEN( "An item is pushed front" ) {
                    l.push_front(100);
                    THEN( "The size increases by 1" ) {
                        REQUIRE( l.size() == 3 );
                    }
                    AND_THEN( "Printing the contents reflects the inserted item")  {
                        std::ostringstream new_contents;
                        l.print(new_contents);
                        REQUIRE( new_contents.str() == "[100,1337,42]" );
                    }
                }
            }
        }

        WHEN( "An item is inserted out of bounds" ) {
            THEN( "The list throws an out of range error" ) {
                REQUIRE_THROWS_AS( l.insert(1234, 1000), std::out_of_range );
            }
        }

        WHEN( "1000 items are inserted using various methods" ) {
            for (int i = 0; i < 1000;) {
                l.insert(++i, 0);
                l.push_back(++i);
                l.push_front(++i);
                l.insert(++i, l.size());
                l.insert(++i, 3);
            }
            THEN( "The size increases by 1000" ) {
                REQUIRE( l.size() == 1000 );
            }
        }
    }
}
