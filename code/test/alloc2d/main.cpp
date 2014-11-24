#include <iostream>

int** alloc2d(const uint rows, const uint cols, const int default_val = 0) {
	int** arr = new int*[rows];
	for (uint row = 0; row != rows; ++row) {
		arr[row] = new int[cols];
		for (uint col = 0; col != cols; ++col) {
			arr[row][col] = default_val;
		}
	}
	return arr;
}

//cant receive const pointer because we could inadvertently modify the underlying data poitned to by the pointer (whose address is constant)
void print_array(int** arr, const uint rows, const uint cols) {
	for (uint row = 0; row != rows; ++row) {
		std::cout << "row " << row << ": ";
		for (uint col = 0; col != cols; ++col) {
			std::cout << arr[row][col] << " ";
		}
		std::cout << std::endl;
	}
}

int main() {
	int** new_arr = alloc2d(5, 10, -5);
	print_array(new_arr, 5, 10);
	return 0;
}