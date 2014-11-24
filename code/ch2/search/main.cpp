#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <math.h>

int sequential_search(std::vector<int> a, int v, int l /*left index*/, int r /*right index*/) {
	for (int i = l; i <= r; ++i) {
		if (v == a[i]) {
			return i;
		}
	}
	return -1;
}

int binary_search(std::vector<int> a, int v, int left, int right) {
	while (right >= left) {
		int middle = (right + left) / 2;
		if (a[middle] == v) {
			return middle;
		} else if (v > a[middle]) {
			//look to the right by moving the left index
			left = middle + 1;
		} else {
			//v < a[middle]
			//look to the left by moving the right index
			right = middle - 1;
		}
	}
	return -1;
}

int main() {
	std::vector<int> N = {1,2,3,4,5,6,7,8,9};
	std::cout << sequential_search(N, 7, 0, N.size() - 1) << std::endl;
	std::cout << binary_search(N, 7, 0, N.size() - 1) << std::endl;

	return 0;
}
