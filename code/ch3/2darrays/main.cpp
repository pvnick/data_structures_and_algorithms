#include <iostream>
#include <math.h>
#include "linked_list.h"

uint** make_arr(uint N, uint M, uint init_val) {
	uint** arr = new uint*[N];
	for (uint i = 0; i < N; ++i) {
		arr[i] = new uint[M];
		for (uint j = 0; j < M; ++j) {
			arr[i][j] = init_val;
		}
	}
	return arr;
}

uint **exercise_64(uint N) {
	uint **arr = make_arr(N, N, 1);
	for (uint gcd = 2; gcd < N; gcd++) {
		for (uint i = gcd; i < N; i += gcd) {
			for (uint j = i; j < N; j += gcd) {
				arr[i][j] = 0;
				arr[j][i] = 0;
			}
		}
	}
	return arr;
}

void verify(uint **arr, const uint N) {
	for (uint i = 2; i < N; i++) {
		for (uint j = 2; j < N; j++) {
			for (uint k = 2; k < i; k++) {
				if (!(i % k) && !(j % k)) {
					if (arr[i][j]) {
						std::cout << i << " " << j << " should be 0" << std::endl;						
					}
				}
			}
		}
	}
}

void print_arr(uint **arr, uint rows, uint cols) {
	for (uint i = 0; i < rows; ++i) {
		std::cout << i << " ";
		if (i < 10) {
			std::cout << " ";
		}
		for (uint j = 0; j < cols; ++j) {
			std::cout << arr[i][j] << "  ";
		}
		std::cout << std::endl;
	}
	std::cout << "   ";
	for (uint j = 0; j < cols; ++j) {
		std::cout << j << " ";
		if (j < 10) {
			std::cout << " ";
		}
	}
	std::cout << std::endl;
}

Linked_List<uint>* matrix_to_multilist(uint **arr, uint N) {
	Linked_List<uint> *list_array = new Linked_List<uint>[N];
	for (uint row = 0; row < N; ++row) {
		for (uint col = 0; col < N; ++col) {
			if (arr[row][col] == 1) {
				list_array[row].add_item(new uint(col));
			}
		}
	}
	return list_array;
}

int main() {
	uint N = 90;
	auto arr = exercise_64(N);
	print_arr(arr, N, N);
	verify(arr, N);
	Linked_List<uint>* lists = matrix_to_multilist(arr, N);
	for (uint i = 0; i < N; i++) {
		std::cout << i << ": " << lists[i].num_nodes() << std::endl;
	}
}