#ifndef _DAVE_HASH_3_
#define _DAVE_HASH_3_

// variation on Program 14.2 from the textbook
// - modified to work with std::string
// for more details see program 12.

#include <cstdlib>
#include <string>

namespace dave_hash_3 {

  std::size_t hash( std::string const& s ) {
    char const* data = s.c_str();
    std::size_t hash;
    std::size_t a = 31415U;
    std::size_t b = 27183U;

    for ( hash = 0; *data != 0; ++data, a = a * b % 25165843U )
      hash = a * hash + *data;
    
    return hash;
  }
}

#endif