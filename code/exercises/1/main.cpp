#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cassert>

template<typename Item>
struct QueueItemContainer {
    Item* item;
    bool is_dummy;
};

template<typename Item>
class Queue {
private:
    int head_index;
    int tail_index;
    QueueItemContainer<Item>* item_container_array;
public:
    Queue(const int size) {
        if (size <= 0) {
            throw std::runtime_error("invalid queue size");
        }
        item_container_array = new QueueItemContainer<Item>[size + 1];
        item_container_array[size].is_dummy = true;
        head_index = 0;
        tail_index = size - 1;
        for (int i = 0; i != size; ++i) {
            item_container_array[i].is_dummy = false;
            item_container_array[i].item = nullptr;
        }
    }
    ~Queue() {
        while (is_empty() == false) {
            Item* item_ptr = dequeue();
            delete item_ptr;
        }
        delete[] item_container_array;        
    }
    const bool is_empty() const {
        if (item_container_array[head_index].item == nullptr) {
            if ((head_index - 1 == tail_index)
                || (head_index == 0 && item_container_array[tail_index + 1].is_dummy)) {
                return true;
            }
        }
        return false;
    }
    const bool is_full() const {
        std::cout << "head=" << head_index << ", tail=" << tail_index << std::endl;
        if (item_container_array[head_index].item != nullptr) {
            if ((head_index - 1 == tail_index)
                || (head_index == 0 && item_container_array[tail_index + 1].is_dummy)) {
                return true;
            }
        }
        return false;
    }
    const int compute_current_queue_size() const {
        int size;
        if (is_empty()) {
            size = 0;
        } else {
            size = 1;
            for (uint i = head_index; i != tail_index; ) {
                if (item_container_array[i].is_dummy) {
                    i = 0;
                } else {
                    ++i;
                    ++size;
                }
            }
        }
        return size;
    }
    void enqueue(Item* new_item) {
        if (is_full()) {
            throw std::runtime_error("queue is full");
        }
        ++tail_index;
        if (item_container_array[tail_index].is_dummy) {
            tail_index = 0;
        }
        item_container_array[tail_index].item = new_item;
    }
    Item* dequeue() {
        if (is_empty()) {
            throw std::runtime_error("queue is empty");
        }
        Item* tmp_head_ptr = item_container_array[head_index].item;
        item_container_array[head_index].item = nullptr;
        ++head_index;
        if (item_container_array[head_index].is_dummy) {
            head_index = 0;
        }
        return tmp_head_ptr;
    }
};

template<typename T>
void run_tests(Queue<T>& q, const int max_queue_size) {
    std::cout << "running tests" << std::endl;
    std::cout << "enqueue: " << std::endl;
    for (int i = 0; i < 7; ++i) {
        std::cout << i << ", " << std::endl;
        q.enqueue(new int(i));
    }
    std::cout << std::endl;
    assert(q.compute_current_queue_size() == 7);
    std::cout << "dequeue: ";
    for (int i = 0; i < 5; ++i) {
        int next_item = *(q.dequeue());
        std::cout << next_item << ", ";
        assert(next_item == i);
    }
    std::cout << std::endl;
    assert(q.compute_current_queue_size() == 2);
    std::cout << "enqueue: ";
    for (int i = 0; i < 7; ++i) {
        std::cout << i << ", ";
        q.enqueue(new int(i));
    }
    std::cout << std::endl;
    std::cout << "dequeue: ";
    assert(q.compute_current_queue_size() == 9);
    while (q.compute_current_queue_size() > 4) {
        int next_item = *(q.dequeue());
        std::cout << next_item << ", ";
    }
    std::cout << std::endl;
    try {
        std::cout << "enqueue: ";
        while (1) {
            std::cout << 31337 << ", ";
            q.enqueue(new int(31337));
        }
    } catch (std::exception e) {
        assert(q.compute_current_queue_size() == max_queue_size);
        std::cout << std::endl;
    }
    try {
        std::cout << "dequeue: ";
        while (1) {
            int next_item = *(q.dequeue());
            std::cout << next_item << ", ";
        }
    } catch (std::exception e) {
        assert(q.compute_current_queue_size() == 0);
        std::cout << std::endl;
    }
    std::cout << "all tests passed" << std::endl;
}

int main() {
    int max_queue_size = 10;
    Queue<int> q(max_queue_size);
    for (uint i = 0; i != 100; ++i) {
        run_tests(q, max_queue_size);
    }
    int* i = new int(123456789);
    q.enqueue(i);
    std::cout << *(q.dequeue()) << std::endl;
}