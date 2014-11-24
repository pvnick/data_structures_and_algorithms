#ifndef _LINKED_LIST_NODE_H
#define _LINKED_LIST_NODE_H
#include <iostream>
#include <stdexcept>

template<typename T>
class Linked_List_Node {
public:
	typedef Linked_List_Node<T> *NodePtr;
	Linked_List_Node(const T i, NodePtr n = nullptr): item(i), next(n) {
		if (n != nullptr) {
			n->prev = this;
		}
	}
	void remove_node() {
		if (prev != nullptr) {
			prev->next = next;
		}
		if (next != nullptr) {
			next->prev = prev;
		}
	}
	NodePtr set_next(NodePtr n) {
		next = n;
		if (n != nullptr) {
			n->prev = this;
		}
		return n;
	}
	NodePtr set_prev(NodePtr p) {
		prev = p;
		if (p != nullptr) {
			p->next = this;
		}
		return p;
	}
	T item;
	NodePtr next;
	NodePtr prev;
	void print_seq(uint max_seq = 20, uint curr_seq = 1) {
		std::cout << item;
		if (next && (max_seq == -1 || curr_seq < max_seq)) {
			std::cout << ", ";
			next->print_seq(max_seq, curr_seq + 1);
		} else {
			std::cout << std::endl;
		}
	}
};

using uint_node_ptr = Linked_List_Node<uint>::NodePtr;


template<typename T>
typename Linked_List_Node<T>::NodePtr node(const T item, typename Linked_List_Node<T>::NodePtr next = nullptr) {
	return typename Linked_List_Node<T>::NodePtr(new Linked_List_Node<T>(item, next));
}


const uint rand_num(uint max);

uint_node_ptr sort_list(uint_node_ptr list);

template<typename T>
typename Linked_List_Node<T>::NodePtr copy_list(typename Linked_List_Node<T>::NodePtr list) {
	auto new_list = node<T>(list->item);
	auto t = list->next;
	auto x = new_list;
	while (t != nullptr) {
		x->next = node<T>(t->item);
		x = x->next;
		t = t->next;
	}
	return new_list;
}

template<typename T>
typename /*typename required because otherwise compiler assumes Linked_List_Node<T> is not a type*/
Linked_List_Node<T>::NodePtr reverse_list(typename Linked_List_Node<T>::NodePtr list) {
	typename Linked_List_Node<T>::NodePtr t, y = list, r = nullptr;
	while (y != nullptr) {
		t = y->next;
		y->next = r;
		r = y;
		y = t;
	}
	return r;
}

template<typename T>
uint num_unique_nodes(typename Linked_List_Node<T>::NodePtr list) {
	//assumes that there are no nullptrs in list (ie node->next always points to itself or another node)
	auto d = list;
	auto s = list;
	uint max_links_before_rejoin;
	do {
		s = s->next;
		d = d->next->next;
		max_links_before_rejoin += 2;
	} while(s != d);
	auto lag = list;
	for (uint i = 0; i < max_links_before_rejoin; lag = lag->next, i++) {
		auto lead = lag->next;
		for (uint j = i; j < max_links_before_rejoin; lead = lead->next, j++) {
			if (lead == lag) {
				return j + 1;
			}
		}
	}
	throw std::runtime_error("wha?");
}


uint_node_ptr generate_random_number_list(const uint N);
uint_node_ptr get_node_by_item_val(const uint_node_ptr list, const uint val);
uint_node_ptr make_sequential_circular_int_linked_list(uint N);
uint_node_ptr make_sequential_int_linked_list(uint N);


#endif