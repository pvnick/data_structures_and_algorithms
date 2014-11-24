#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

void coin_flips_quadratic_time(const double head_probability, const uint N /*coin flips per experiment*/, const uint M /*num experiments*/);

uint heads(double p) {
	return rand() < RAND_MAX * p;
}

int main() {
	//coin_flips_quadratic_time(1.0 / 6.0, 32, 1000);
	double lambda = 10;
	coin_flips_quadratic_time(lambda / 100, 32, 100);

}

//following function is O(N^2)
void coin_flips_quadratic_time(const double head_probability, const uint N /*coin flips per experiment*/, const uint M /*num experiments*/) {
	uint i, j, cnt;
	std::vector<uint> f(N + 1, 0); //histogram holder
	//run experiments
	for (i = 0; i < M; ++i, ++f[cnt]) {
		//an experiment
		for (cnt = 0, j = 0; j < N; ++j) {
			heads(head_probability) && ++cnt;
		}
	}
	//print the histogram
	for (j = 0; j < N; ++j) {
		if (!f[j]) {
			std::cout << "." << std::endl;
		} else {
			cnt = f[j];
			std::string hist_bin(cnt, '*');
			std::cout << hist_bin << std::endl;
		}
	}
}