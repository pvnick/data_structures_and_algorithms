#pragma once

template<class Item>
class Node {
public:
	Node(Item *i): item(i), next(nullptr) {}
	const Item* item;
	Node<Item> *next;
};