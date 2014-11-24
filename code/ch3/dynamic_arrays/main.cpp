#include <iostream>
#include <string>
#include <sstream>

std::string** alloc2d(uint rows, uint cols) {
	std::string** arr = new std::string*[rows];
	for (uint row = 0; row != rows; ++row) {
		arr[row] = new std::string[cols];
	}
	return arr;
}

int main() {
	std::cout << "hello" << std::endl;
	uint rows = 10, cols = 5;
	std::string** arr2d = alloc2d(rows, cols);
	for (uint row = 0; row != rows; ++row) {
		for (uint col = 0; col != cols; ++col) {
			std::ostringstream strstream;
			strstream << (row * cols + col);
			arr2d[row][col] = strstream.str(); 
		}
	}
	for (uint row = 0; row != rows; ++row) {
		for (uint col = 0; col != cols; ++col) {
			std::cout << arr2d[row][col] << std::endl; 
		}
	}
	return 0;
}