#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>


void sieve_of_eratosthenes_bitwise(const uint);
void sieve_of_eratosthenes_plot(const uint);
void sieve_of_eratosthenes(const uint);

int main() {
	sieve_of_eratosthenes_plot(100);
	return 0;
}

void sieve_of_eratosthenes_bitwise(const uint N) {
	uint i;
	uint max_ull_bits = 64;
	uint max_ulls_required_to_hold_N_bits = 1 + N / max_ull_bits; //could just need max_ulls_required_to_hold_N_bits - 1, but who cares
	uint vector_element;
	uint bits_to_shift;
	unsigned long long shifted;
	std::vector<unsigned long long> a_v(max_ulls_required_to_hold_N_bits);
	for (i = 2; i != N; ++i) {
		vector_element = i / 64;
		bits_to_shift = (i % 64);
		shifted = static_cast<unsigned long long>(1) << bits_to_shift;
		a_v[vector_element] |= shifted;
	}
	for (i = 2; i < sqrt(N); ++i) {
		vector_element = i / 64;
		bits_to_shift = (i % 64);
		shifted = static_cast<unsigned long long>(1) << bits_to_shift;
		if (a_v[vector_element] & shifted) {
			for (uint j = i; j * i < N; ++j) {
				vector_element = i * j / 64;
				bits_to_shift = ((i * j) % 64);
				a_v[vector_element] &= ~(static_cast<unsigned long long>(1) << bits_to_shift);
			}
		}
	}
	uint num_primes = 0;
	for (i = 2; i < N; ++i) {
		vector_element = i / 64;
		bits_to_shift = (i % 64);
		shifted = static_cast<unsigned long long>(1) << bits_to_shift;
		if (a_v[vector_element] & shifted) {
			num_primes++;
			//std::cout << " " << i;
		}
	}
	std::cout << num_primes << std::endl;
}


void sieve_of_eratosthenes(const uint N) {
	uint i;
	uint *a_v = new uint[N];
	for (i = 2; i != N; ++i) {
		a_v[i] = 1;
	}
	for (i = 2; i < sqrt(N); ++i) {
		if (a_v[i]) {
			for (uint j = i; j * i < N; ++j) {
				a_v[i*j] = 0;
			}
		}
	}
	uint num_primes = 0;
	for (i = 2; i < N; ++i) {
		if (a_v[i]) {
			num_primes++;
		}
	}
	std::cout << num_primes << std::endl;
	delete[] a_v;
}

void sieve_of_eratosthenes_plot(const uint N) {
	std::vector<uint> a(N, 1);
	a[1] = 0;
	for (uint i = 2; i < sqrt(N); ++i) {
		if (a[i]) {
			for (uint j = i; i * j < N; ++j) {
				a[i*j] = 0;
			}			
		}
	}
	std::string prime_count_bar;
	for (uint i = 1; i < N; ++i) {
		std::cout << "# less than " << i << ": " << prime_count_bar << std::endl;
		if (a[i]) {
			prime_count_bar += "*";
		}
	}
}