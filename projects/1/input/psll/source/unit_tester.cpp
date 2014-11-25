#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "PSLL.h"

#define INSTANTIATE_LIST( T, OBJ_NAME ) cop3530::PSLL<T> OBJ_NAME
#undef INTERNAL_CATCH_UNIQUE_NAME_LINE2
#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) PSLL##name##line
#define LIST "PSLL"

#include "insert.h"
#include "remove.h"
#include "random_access.h"
