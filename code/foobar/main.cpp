#include <iostream>
#include "ll_queue.h"
#include <string>

using namespace std;

int main()
{
	LLQueue<std::string> list;
	for (uint i = 0; i < 5 ; i++) 
		list.enqueue(std::string("first"));
	list.enqueue(std::string("second"));
	list.enqueue(std::string("third"));
	std::cout << (list.dequeue()) << std::endl;
	std::cout << (list.dequeue()) << std::endl;
	std::cout << (list.dequeue()) << std::endl;
	list.enqueue(std::string("third"));
	std::cout << (list.dequeue()) << std::endl;
    return 0;
}
