#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdexcept>

template <typename T>
class Queue {
private:
    struct Node {
        T item;
        Node* next;
    };
    Node* head = nullptr;
    Node* tail = nullptr;
    void delete_list() {
        for (Node* n = head; ! is_empty(); head = n) {
            n = head->next;
            delete head;
        }
        head = tail = nullptr;
    }
    void copy_queue(const Queue& other) {
        for (Node* n = other.head; n != nullptr; n = n->next) {
            T item = n->item;
            put(item);
        }
    }
public:
    Queue() = default;
    ~Queue() {
        delete_list();
    }
    Queue(const Queue& other) {
        //copy constructor
        copy_queue(other);
    };
    Queue& operator=(const Queue& other) {
        //copy assignment
        if (this == &other)
            return *this;

        delete_list();
        copy_queue(other);
        return *this;
    };
    int is_empty() const {
        return (head == nullptr);
    }
    void put(T item) {
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
    T get() {
        if (is_empty()) {
            throw std::underflow_error("can't pop: list is empty");
        }
        Node* n = head;
        T item = n->item;
        head = n->next;
        delete n;
        return item;
    }
};

#endif // _QUEUE_H
