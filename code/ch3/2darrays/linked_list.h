#pragma once

#include "node.h"

template<class Item>
class Linked_List {
public:
	Linked_List(): head(nullptr) {};
	void add_item(Item *i) {
		Node<Item> *n = new Node<Item>(i);
		if (head == nullptr) {
			head = n;
		} else {
			n->next = head;
			head = n;
		}
	}
	uint num_nodes() {
		uint count = 0;
		for (auto traverser = head; traverser->next != nullptr; ++count, traverser = traverser->next);
		return count;
	}
private:
	Node<Item> *head;
};