#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "SDAL.h"
#include "list_tester.h"

#define INSTANTIATE_LIST( T, OBJ_NAME ) cop3530::ListTester<cop3530::SDAL<T>> OBJ_NAME
#define FINALLY_VALIDATE_LIST_INTEGRITY( OBJ_NAME ) \
    AND_THEN( "The list's internal integrity is validated") { \
        REQUIRE( OBJ_NAME.validate_internal_integrity() ); \
    }
#define LIST_NAME "SDAL"

#include "insert.h"
