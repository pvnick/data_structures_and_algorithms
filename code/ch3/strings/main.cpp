#include <iostream>
#include <cstring>

bool is_palendrome(char str[]) {
	uint len = std::strlen(str);
	char* beg_ptr = str;
	char* end_ptr = beg_ptr + len - 1;
	for (; beg_ptr < end_ptr; beg_ptr++, end_ptr--) {
		for (; *beg_ptr == ' '; ++beg_ptr); 
		for (; *end_ptr == ' '; --end_ptr);
		if (beg_ptr >= end_ptr) {
			return true;
		}
		else if (*beg_ptr != *end_ptr) {
			return false;
		}

	}
	return true;
}

int main() {
	std::cout << "hello" << std::endl;
	std::cout << is_palendrome("asdfklasdofas") << std::endl;
	return 0;
}