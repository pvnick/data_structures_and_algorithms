#ifndef _COMMON_H_
#define _COMMON_H_

double lg(size_t i) {
	return std::log(i) / std::log(2);
}

static constexpr double phi = 1.61803398875;
static constexpr double fib_hash_A = 1 / phi;

#endif