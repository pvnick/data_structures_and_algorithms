#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CDAL.h"

#define INSTANTIATE_LIST( T, OBJ_NAME ) cop3530::CDAL<T> OBJ_NAME
#undef INTERNAL_CATCH_UNIQUE_NAME_LINE2
#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) CDAL##name##line
#define LIST "CDAL"

#include "insert.h"
#include "remove.h"
#include "random_access.h"
