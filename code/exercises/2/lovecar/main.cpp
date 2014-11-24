#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

typedef unsigned int uint;

template<typename Item>
struct Node {
	bool holds_object;
	Item i;
};

template<typename Item>
class Stack {
public:
	Stack(uint max_size): size(max_size) {
		items = new Node<Item>[size];
		head_index = 0;
	};
	void push(Item i) {
		if ( ! is_empty()) {
			head_index++;
		}
		items[head_index].i = i;
		items[head_index].holds_object = true;
	}
	bool is_empty() {
		return head_index == 0 && items[head_index].holds_object == false;
	}
	Item pop() {
		if (is_empty()) {
			throw std::runtime_error("cant pop from an empty stack");
		}
		Item i = items[head_index].i;
		items[head_index].holds_object = false;
		if ( ! is_empty()) {
			head_index--;
		}
		return i;
	}
	Item peak() {
		Item i = pop();
		push(i);
		return i;
	}
	void operator<<(Item& i) {
		push(i);
	}
	void operator<<(Stack<Item>& src) {
		push(src.pop());
	}
	void operator>>(Item& target) {
		target = pop();
	}
private:
	uint head_index;
	Node<Item>* items;
	uint size;
};

template<typename Item>
std::istream& operator>>(std::istream& is, Stack<Item>& target) {
	Item i;
	is >> i;
	target << i;
	if ( ! is) {
		throw std::runtime_error("Bad input");
	}
	return is;
}


template<typename Item>
class Queue {
public:
	Queue(const uint size): arr_size(size), 
							head_index(0), 
							tail_index(0), 
							_count(0) {
		arr = new Node<Item>[size];
	}
	bool is_empty() {
		return count() == 0;
	}
	uint count() {
		return _count;
	}
	bool is_full() {
		return count() == arr_size;
	}
	void enqueue(Item i) {
		if (is_full()) {
			throw std::runtime_error("full queue");
		}
		Node<Item>& n = arr[tail_index];
		n.i = i;
		n.holds_object = true;
		tail_index = (tail_index + 1) % arr_size;
		++_count;
	}
	Item dequeue() {
		if (is_empty()) {
			throw std::runtime_error("empty queue");
		}
		Node<Item>& n = arr[head_index];
		Item ret = n.i;
		n.holds_object = false;
		head_index = (head_index + 1) % arr_size;
		--_count;
		return ret;
	}
	void operator>>(Stack<Item>& target) {
		Item i = dequeue();
		target.push(i);
	}
	void operator>>(Item& target) {
		target = dequeue();
	}
	void operator<<(Item i) {
		enqueue(i);
	}
	const uint arr_size;
private:
	Node<Item>* arr;
	uint head_index;
	uint tail_index;
	uint _count;
};

template<typename Item>
std::istream& operator>>(std::istream& is, Queue<Item>& target) {
	Item i;
	is >> i;
	target << i;
	if ( ! is) {
		throw std::runtime_error("Bad input");
	}
	return is;
}

bool can_rearrange(uint n, Queue<uint> approach) {
	Stack<uint> sidestreet(n);
	Stack<uint> away(n);
	while (approach.is_empty() == false) {
		uint car;
		approach >> car;
		uint last_sent_car = away.is_empty() ? 0 : away.peak();
		if (car == last_sent_car + 1) {
			//next in line
			away << car;
		} else {
			//out of order
			if (sidestreet.is_empty() || sidestreet.peak() > car) {
				sidestreet << car;
			} else if (sidestreet.peak() == last_sent_car + 1) {
				while ( ! sidestreet.is_empty() 
						&& sidestreet.peak() == last_sent_car + 1) {
					sidestreet >> last_sent_car;
					away << last_sent_car;
				}
				sidestreet << car;	
			} else {
				return false;
			}
		}
	}
	return true;
}

bool testcase(uint n) {
	Queue<uint> approaching_cars(n);
	for (uint i = 0; i != n; ++i) {
		std::cin >> approaching_cars;
	}
	return can_rearrange(n, approaching_cars);
}

int main() {
	uint n;
	Queue<bool> results(1000000 /*this should really be a linked list queue*/);
	while ((std::cin >> n) && n != 0) {
		results << testcase(n);
	}
	while ( ! results.is_empty()) {
		std::cout << (results.dequeue() ? "yes" : "no") << std::endl;
	}
	return 0;
}