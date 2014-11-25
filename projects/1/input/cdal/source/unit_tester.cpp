#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CDAL.h"

#define INSTANTIATE_LIST( T, OBJ_NAME ) cop3530::CDAL<T> OBJ_NAME

#include "insert.h"
#include "remove.h"
