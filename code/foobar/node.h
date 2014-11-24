#ifndef _NODE_H
#define _NODE_H

template<class Item>
class Node {
public:
	Node(Item *i = nullptr, const bool dummy = true): is_dummy(dummy), item(i), next(nullptr) {};
	const Item* get_item() const {return item;}
	void set_next(Node<Item> *n) { next = n; }
	Node<Item> * get_next() { return next; }
	void convert_to_nondummy(Item* i) {
		std::cout << "hi1" << std::endl;
		//if (item)
		//	std::cout << *item << std::endl;
		//item = i; 
		std::cout << "hi" << std::endl;
		std::cout << "2" << std::endl;
		is_dummy = false;
	}
	void convert_to_dummy() {
		if ( ! is_dummy_node()) {
			destruct_item(); 
			is_dummy = true;
		}
	}
	const bool is_dummy_node() const {return is_dummy;}
private:
	void destruct_item() {delete item; item = nullptr;}
    bool is_dummy;
    Item *item;
    Node<Item> *next;
};

#endif
