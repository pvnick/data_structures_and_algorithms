#include "queue.h"
#include <stdexcept>

template<typename T>
int Queue<T>::is_empty() const {
    return (head == nullptr);
}

template<typename T>
void Queue<T>::put(T item) {
    Node* n = new Node();
    n->item = item;
    n->next = nullptr;
    if (is_empty()) {
        head = tail = n;
    } else {
        tail->next = n;
        tail = n;
    }
}

template<typename T>
T Queue<T>::get() {
    if (is_empty()) {
        throw std::underflow_error("can't pop: list is empty");
    }
    Node* n = head;
    T item = n->item;
    head = n->next;
    delete n;
    return item;
}

