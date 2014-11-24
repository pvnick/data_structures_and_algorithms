#include <iostream>
#include "Linked_List_Node.h"

typedef std::string s;

void exercise_25(uint_node_ptr x, uint_node_ptr t) {
	//given x and t (two disjoint circular lists), this inserts t into x
	uint_node_ptr t_beginning = t;
	uint_node_ptr x_beginning = x;
	uint_node_ptr t_end = t_beginning;
	for (; t_end->next != t_beginning; t_end = t_end->next);
	t_end->next = x->next;
	x->next = t_beginning;
}

void exercise_26(uint_node_ptr x, uint_node_ptr t) {
	//given x and t, both pointers to nodes on the same circular list, moves the node following t
	//to the position following the node following x
	auto node_after_t = t->next;
	//remove node_after_t
	t->next = t->next->next;
	//insert after node after x
	auto node_after_x = x->next;
	auto node_after_node_after_x = node_after_x->next;
	node_after_x->next = node_after_t;
	node_after_t->next = node_after_node_after_x;
}

void josephus(uint N, uint M) {
	uint_node_ptr t = node<uint>(1);
	auto x = t;
	for (uint i = 2; i <= N; ++i) {
		auto new_x = node<uint>(i);
		x->next = new_x;
		x = new_x;
	} 
	x->next = t;
	std::cout << x->item << std::endl;
	while (x != x->next) {
		for (uint i = 1; i < M; ++i) {
			x = x->next;
		}
		std::cout << x->next->item << " eliminated" << std::endl;
		x->next = x->next->next;
	}
	std::cout << "leader:" << x->item << std::endl;
}


void tests() {
	auto end = node(s("end"));
	auto middle = node(s("middle"), end);
	auto beginning = node(s("beginning"), middle);
	end->next = beginning;
	beginning->print_seq();

	//remove middle node
	beginning->next = beginning->next->next;
	beginning->print_seq();

	//insert a node
	auto new_middle = node(s("new middle"), end);
	beginning->next = new_middle;
	beginning->print_seq();
}


uint_node_ptr make_sequential_circular_int_linked_list(uint N) {
	uint_node_ptr root = node<uint>(1);
	root->next = root;
	uint_node_ptr x = root;
	for (uint i = 2; i < N; ++i) {
		x = (x->next = node<uint>(i, root)); 
	}
	return root;
}

uint_node_ptr make_sequential_int_linked_list(uint N) {
	uint_node_ptr root = node<uint>(1);
	uint_node_ptr x = root;
	for (uint i = 2; i <= N; ++i) {
		auto n = node<uint>(i);
		x = x->set_next(n);
	}
	return root;
}


int main() {
	/*auto list1 = make_sequential_circular_int_linked_list(10);
	auto list2 = make_sequential_circular_int_linked_list(20);
	exercise_25(list1, list2);
	list1->print_seq(40);
	
	auto list3 = make_sequential_circular_int_linked_list(10);
	exercise_26(get_node_by_item_val(list3, 3), get_node_by_item_val(list3, 7));
	list3->print_seq(40);*/

	uint_node_ptr list = make_sequential_int_linked_list(100);
	get_node_by_item_val(list,100)->set_next(get_node_by_item_val(list,100));
	list->print_seq(num_unique_nodes<uint>(list));
	std::cout << std::strlen("asdfasdfasdf") << std::endl;
	return 0;
}