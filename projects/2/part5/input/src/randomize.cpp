// randomize.cpp
//
// - reads N lines from std::cin and outputs them, in "random" 
//   order, to std::cout
//
// usage: randomize N < input.txt > output.txt
//
// Note: I am deliberately not seeding the RNG, so that the output order
// will be reproducable between runs for a given N.  Should that not be
// desirable for your application, uncomment the call to srand() in main().
//
// by Dave Small
// 201007.29 v1.0 - created
// 201007.30 v1.0.1 - cleaned up code for release

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>

// better random numbers from an LC RNG
// source: pp135-136 of _Accelerated C++_ by Koenig and Moo
int nrand( int n ) {
  const int bucket_size = RAND_MAX / n;
  int r;
  
  do
    r = rand() / bucket_size;
  while ( r >= n );
  
  return r;
}

// Fisher-Yates Shuffle
template <typename T>
void shuffle( T* a, int size ) {
  for ( int i = size-1; i > 0; --i ) {
    int r = nrand( i );
    T tmp = a[r];
    a[r] = a[i];
    a[i] = tmp;
  }
}
    
int main( int argc, char** argv ) {
  int lines = atoi( argv[1] );

  int index[ lines ];
  for ( int i = 0; i < lines; ++i )
    index[i] = i;

  //srand( time(NULL) );  // uncomment line for different results on each run
  shuffle<int>( index, lines );

  std::string data[ lines ];
  for ( int i = 0; i < lines; ++i )
    getline( std::cin, data[i] );

  for ( int i = 0; i < lines; ++i )
    std::cout << data[ index[i] ] << std::endl;

  return 0;
}


