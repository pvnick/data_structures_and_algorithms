#include <iostream>
#include <time.h>
#include <string>
#include <vector>

void do_loop(int N);

int main() {
	time_t init_time, fin_time;
	std::vector<int> N_vals = {1500}; //{1000, 2000, 2500};
	for (auto N: N_vals) {
		time(&init_time);
		do_loop(N);
		time(&fin_time);
		std::cout << "N=" << N << " took " << (fin_time - init_time) << "seconds" << std::endl;
	}
	return 0;
}

void do_loop(int N) {
	int i, j, k, count = 0;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			for (k = 0; k < N; k++) {
				count++;
			}
		}
	}
}