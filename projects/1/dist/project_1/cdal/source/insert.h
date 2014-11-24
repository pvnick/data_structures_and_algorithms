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
            l.insert(0, 0);
            THEN( "The size increases by 1" ) {
                REQUIRE( l.size() == 1 );
            }
            AND_THEN( "Printing the contents reflects the single inserted item")  {
                std::ostringstream new_contents;
                l.print(new_contents);
                REQUIRE( new_contents.str() == "[0]" );
            }
            FINALLY_VALIDATE_LIST_INTEGRITY(l)
        }

        WHEN( "1000 items are inserted" ) {
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
            FINALLY_VALIDATE_LIST_INTEGRITY(l)
        }
    }
}
