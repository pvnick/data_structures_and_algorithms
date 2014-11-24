#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdlib.h>
#include "node.h"
#include <stdexcept>

template<class Item>
class LLQueue {
public:
	LLQueue(): free(nullptr), number_of_free_nodes(0) {
		Node<Item>* dummy = new_node();
		head = tail = dummy;
		populate_free_nodes();
	}
	void populate_free_nodes() {
		Node<Item> **nodes = new Node<Item>*[max_number_of_free_nodes];
		for (uint i = 0; i < max_number_of_free_nodes; ++i) {
			nodes[i] = new_node();
		}
		for (uint i = 0; i < max_number_of_free_nodes; ++i) {
			free_node(nodes[i]);
		}
	}
	void enqueue(Item i) {
		Item *i_ptr = new Item(i);
		Node<Item> *new_dummy = new_node();
		tail->convert_to_nondummy(i_ptr);
		tail->set_next(new_dummy);
		tail = new_dummy;
	}
	const Item dequeue() {
		Node<Item> *n = head;
		if (n->is_dummy_node()) {
			throw std::overflow_error("no more items in queue");
		}
		head = n->get_next();
		const Item ret = *(n->get_item());
		free_node(n);
		return ret;
	}
private:
	static uint max_number_of_free_nodes;
	Node<Item> *head;
	Node<Item> *tail;
	Node<Item> *free; //pool
	uint number_of_free_nodes;
	Node<Item> *new_node() {
		if (free == nullptr) {
			return new Node<Item>();
		} else {
			auto n = free;
			free = n->get_next();
			--number_of_free_nodes;
			return n;
		}
	}
	void free_node(Node<Item> *n) {
		n->convert_to_dummy();
		n->set_next(free);
		free = n;
		++number_of_free_nodes;
	}
};

template<class Item>
uint LLQueue<Item>::max_number_of_free_nodes = 10;


#endif