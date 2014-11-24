#include "Linked_List_Node.h"

const uint rand_num(uint max = RAND_MAX) {
	return rand() % max;
}

uint_node_ptr sort_list(uint_node_ptr list) {
	//output
	auto head = node<uint>(0);
	uint_node_ptr t,u,x;
	//loop through every node in input 
	for (t = list->next; t != nullptr; t = u) {
		//save pointer to node after t
		u = t->next;
		x = head;
		//loop through every node in output, find a node to insert t after
		for (; x->next != nullptr; x = x->next) {
			if (x->next->item > t->item) {
				//current node is either headnode or less than or equal to t, and next item is greater than t
				break;
			}
		}
		t->next = x->next;
		x->next = t;
	}
	return head;
}



uint_node_ptr generate_random_number_list(const uint N) {
	uint_node_ptr head = node<uint>(0), x = head;
	for (uint i = 0; i != N; ++i) {
		x->next = node<uint>(rand_num(100));
		x = x->next;
	}
	return head;
}

uint_node_ptr get_node_by_item_val(const uint_node_ptr list, const uint val) {
	auto x = list;
	do {
		if (x->item == val) {
			return x;
		} 
		x = x->next;
	} while(x != list);
	return nullptr;
}

