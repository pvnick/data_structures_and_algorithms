#include <iostream>
#include "ll_queue.h"
#include <string>

using namespace std;

int main()
{
	LLQueue<std::string> list;
	for (uint i = 0; i < 20 ; i++) 
		list.enqueue(std::string("first"));
	list.enqueue(std::string("second"));
	list.enqueue(std::string("third"));
	std::cout << (list.dequeue()) << std::endl;
	std::cout << (list.dequeue()) << std::endl;
	std::cout << (list.dequeue()) << std::endl;
	list.enqueue(std::string("third"));
	std::cout << (list.dequeue()) << std::endl;
	try {
		while(1) 
			std::cout << list.dequeue() << std::endl;
	} catch(std::exception e){}
    return 0;
}
