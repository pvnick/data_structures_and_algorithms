#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "SDAL.h"

#define INSTANTIATE_LIST( T, OBJ_NAME ) cop3530::SDAL<T> OBJ_NAME
#undef INTERNAL_CATCH_UNIQUE_NAME_LINE2
#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) SDAL##name##line
#define LIST "SDAL"

#include "insert.h"
#include "remove.h"
#include "random_access.h"
