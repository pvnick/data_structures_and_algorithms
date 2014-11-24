#ifdef _UNIT_TESTS_H
#error Only include unit_tests.h once!
#endif // _UNIT_TESTS_H
#define _UNIT_TESTS_H

#include "src/CDAL.h"
#include "src/SSLL.h"
#include "src/SDAL.h"
#include "src/PSLL.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "ListTester.h"

#define INSTANTIATE_LIST( T, OBJ_NAME ) cop3530::ListTester<cop3530::LIST<T>> OBJ_NAME
#define FINALLY_VALIDATE_LIST_INTEGRITY \
    AND_THEN( "The list's internal integrity is validated") { \
        REQUIRE( l.validate_internal_integrity() ); \
    }

#define LIST SSLL
#define NAME_PREFIX "SSLL: "
#undef INTERNAL_CATCH_UNIQUE_NAME_LINE2
#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) SSLL##name##line
#include "all_tests.h"
#undef LIST
#undef NAME_PREFIX

#define LIST SDAL
#define NAME_PREFIX "SDAL: "
#undef INTERNAL_CATCH_UNIQUE_NAME_LINE2
#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) SDAL##name##line
#include "all_tests.h"
#undef LIST
#undef NAME_PREFIX

#define LIST CDAL
#define NAME_PREFIX "CDAL: "
#undef INTERNAL_CATCH_UNIQUE_NAME_LINE2
#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) CDAL##name##line
#include "all_tests.h"
#undef LIST
#undef NAME_PREFIX

#define LIST PSLL
#define NAME_PREFIX "PSLL: "
#undef INTERNAL_CATCH_UNIQUE_NAME_LINE2
#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) PSLL##name##line
#include "all_tests.h"
#undef LIST
#undef NAME_PREFIX
