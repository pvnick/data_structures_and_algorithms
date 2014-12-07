#define _DEBUG_ false
#include <iostream>
#include "avl.h"

int main() {
cop3530::AVL<int, int> map(10);
map.insert(1,3);
map.insert(4,3);
map.insert(6,3);
map.insert(2,3);
map.print(std::cout);
}
