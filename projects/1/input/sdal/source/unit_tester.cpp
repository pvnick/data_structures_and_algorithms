#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "SDAL.h"

#define INSTANTIATE_LIST( T, OBJ_NAME ) cop3530::SDAL<T> OBJ_NAME

#include "insert.h"
#include "remove.h"
