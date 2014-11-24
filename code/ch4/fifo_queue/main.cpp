#include <iostream>
#include <string>
#include <stdexcept>
#include <iomanip>

template <typename T>
class LLQueue {
private:
    struct Node {
        T item;
        Node* next;
        Node(T i) {
            item = i;
            next = nullptr;
        }
    };

    Node* head;
    Node* tail;
    size_t node_count;
public:
    bool is_empty() {
        return size() == 0;
    }
    void enqueue(T item) {
        //enqueueing adds to the *tail*
        Node* n = new Node(item);
        if (tail == nullptr) {
            //empty list
            head = tail = n;
        } else {
            tail->next = n;
            tail = n;
        }
        ++node_count;
    }
    T dequeue() {
        //dequeueing takes from the *head*
        if (is_empty()) {
            throw std::underflow_error("Cant dequeue: queue is empty");
        }
        Node* n = head;
        T item = n->item;
        head = n->next;
        delete n;
        --node_count;
        return item;
    }
    size_t size() {
        return node_count;
    }
    void clear() {
        while( ! is_empty()) {
            dequeue();
        }
    }
    LLQueue(): head(nullptr), tail(nullptr), node_count(0) {}
    ~LLQueue() {
        clear();
    }
};

template <typename T>
class ArrQueue {
private:
    struct Node {
        T item;
        bool is_dummy;
    };
    Node* arr;
    int head_index;
    int tail_index;
public:
    ArrQueue(size_t array_size = 50) {
        arr = new Node[array_size + 1];
        arr[array_size].is_dummy = true;
        tail_index = head_index = 0; //again, not initializing these up-front was causing memory corruption
    }
    void enqueue(T item) {
        ++tail_index;
        if (arr[tail_index].is_dummy)
            tail_index = 0;
        arr[tail_index].item = item;
    }
    T dequeue() {
        ++head_index;
        if (arr[head_index].is_dummy)
            head_index = 0;
        return arr[head_index].item;
    }
};

template <typename T>
class Deque {
private:
    struct Node {
        T item;
        Node* next;
        Node* prev;
        bool is_dummy;
    };
    Node* head;
    Node* tail;
public:
    Deque() {
        head = new Node();
        tail = new Node();
        head->is_dummy = true;
        head->next = tail;
        tail->is_dummy = true;
        tail->prev = head;
    }
    void push_front(T item) {
        Node* n = new Node();
        n->item = item;
        n->next = head->next;
        n->prev = head;
        head->next->prev = n;
        head->next = n;
    }
    T pop_front() {
        if (is_empty()) {
            throw std::underflow_error("cant pop: list is empty");
        }
        Node* n = head->next;
        T item = n->item;
        head->next = head->next->next;
        head->next->prev = head;
        delete n;
        return item;
    }
    void push_back(T item) {
        Node* n = new Node();
        n->item = item;
        n->next = tail;
        n->prev = tail->prev;
        tail->prev->next = n;
        tail->prev = n;
    }
    T pop_back() {
        if (is_empty()) {
            throw std::underflow_error("cant pop: list is empty");
        }
        Node* n = tail->prev;
        T item = n->item;
        tail->prev = tail->prev->prev;
        tail->prev->next = tail;
        delete n;
        return item;
    }
    bool is_empty() {
        return head->next == tail;
    }
};

int randi(int min, int max) {
    return min + (rand() % max);
}

template<typename T>
class RandomQueue {
private:
    const int num_slots;
    int end_index; //one past last filled slot
    T* arr;
public:
    RandomQueue(int max_size = 50): num_slots(max_size), end_index(0) {
        arr = new T[num_slots];
    }
    void put(T item) {
        arr[end_index++] = item;
    }
    T get() {
        int rand_index = randi(0, end_index--);
        T item = arr[rand_index];
        std::swap(arr[rand_index], arr[end_index]);
        return item;
    }
};

int main()
{
    ArrQueue<std::string> q(2);
    q.enqueue(std::string("foobar"));
    q.enqueue(std::string("hello"));
    std::cout << q.dequeue() << std::endl;
    q.enqueue("world");
    std::cout << q.dequeue() << std::endl;
    std::cout << q.dequeue() << std::endl;

    LLQueue<std::string> q2;
    q2.enqueue(std::string("foobar"));
    q2.enqueue(std::string("hello"));
    std::cout << q2.dequeue() << std::endl;
    q2.enqueue("world");
    std::cout << q2.dequeue() << std::endl;
    std::cout << q2.dequeue() << std::endl;

    Deque<const char*> q3;
    q3.push_front("hello");
    q3.push_back("world");
    q3.push_front("foo");
    q3.push_back("bar");
    std::cout << q3.pop_front() << std::endl;
    std::cout << q3.pop_back() << std::endl;
    q3.push_front("baz");
    std::cout << q3.pop_back() << std::endl;
    std::cout << q3.pop_back() << std::endl;
    std::cout << q3.pop_back() << std::endl;

    RandomQueue<int> rq;
    for (int i = 0; i < 50; ++i)
        rq.put(i);

    int histogram[50];

    for (int i = 0; i < 50; ++i) {
        histogram[i] = 0;
        rq.put(i);
    }

    for (int i = 0; i < 10000; ++i) {
        int result = rq.get();
        histogram[result]++;
        rq.put(result);
    }

    int max_count = 0;
    for (int i = 0; i < 50; ++i) {
        if (histogram[i] > max_count)
            max_count = histogram[i];
    }

    for (int i = 0; i < 50; ++i) {
        int count = histogram[i];
        int bar_size = 100 * count / max_count;
        std::cout << std::setfill('0') << std::setw(2) << i << ": ";
        for (int j = 0; j < bar_size; ++j)
            std::cout << "*";
        std::cout << std::endl;
    }
    return 0;
}
