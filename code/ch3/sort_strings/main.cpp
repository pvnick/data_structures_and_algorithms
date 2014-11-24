#include <sstream>
#include <iostream>
#include <string>
#include <stdlib.h>

int compare(const void *str1, const void *str2) {
	if (*(std::string*)str1 == *(std::string*)str2) {
		return 0;
	} else if (*(std::string*)str1 < *(std::string*)str2) {
		return -1;
	} else {
		return 1;
	}
}

int main() {
	const uint num_strs = 50;
	std::string* strs = new std::string[num_strs];
	for (std::string* str_ptr = strs; str_ptr != strs + num_strs; ++str_ptr) {
		std::ostringstream str_stream;
		str_stream << rand();
		*str_ptr = str_stream.str();
	}
	std::qsort(strs, num_strs, sizeof(std::string*), compare);
	for (std::string* str_ptr = strs; str_ptr != strs + num_strs; ++str_ptr) {
		std::cout << *str_ptr << std::endl;
	}
	
	return 0;
}