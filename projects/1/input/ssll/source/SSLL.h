//note to self: global search for todo and xxx before turning this assignment in












#ifndef _SSLL_H_
#define _SSLL_H_

// SSLL.H
//
// Singly-linked list (non-polymorphic)
//
// Authors: Paul Nickerson, Dave Small
// for COP 3530
// 201409.16 - created

#include <iostream>
#include <stdexcept>
#include <cassert>

namespace cop3530 {
    template <class T>
    class SSLL {
    private:
        struct Node {
            T item;
            Node* next;
            bool is_dummy;
        }; // end struct Node
        size_t num_items;
        Node* head;
        Node* tail;
        Node* node_at(size_t position) const {
            Node* n = head->next;
            for (size_t i = 0; i != position; ++i, n = n->next);
            return n;
        }
        Node* node_before(size_t position) const {
            if (position == 0)
                return head;
            else
                return node_at(position - 1);
        }
        Node* design_new_node(const T& element, Node* next = nullptr, bool dummy = false) {
            Node* n = new Node();
            n->is_dummy = dummy;
            n->item = element;
            n->next = next;
            return n;
        }
        Node* design_new_node(Node* next = nullptr, bool dummy = false) {
            Node* n = new Node();
            n->is_dummy = dummy;
            n->next = next;
            return n;
        }
        void init() {
            num_items = 0;
            try {
                tail = design_new_node(nullptr, true);
                head = design_new_node(tail, true);
            } catch (std::bad_alloc& ba) {
                std::cerr << "init(): failed to allocate memory for head/tail nodes" << std::endl;
                throw std::bad_alloc();
            }
        }
        //note to self: the key to simple ssll navigation is to frame the problem in terms of the following two functions (insert_node_after and remove_item_after)
        void insert_node_after(Node* existing_node, Node* new_node) {
            existing_node->next = new_node;
            ++num_items;
        }
        //destroys the subsequent node and returns its item
        T remove_item_after(Node* preceeding_node) {
            Node* removed_node = preceeding_node->next;
            T item = removed_node->item;
            preceeding_node->next = removed_node->next;
            delete removed_node;
            --num_items;
            return item;
        }
        void copy_constructor(const SSLL& src) {
            const_iterator fin = src.end();
            for (const_iterator iter = src.begin(); iter != fin; ++iter) {
                push_back(*iter);
            }
        }
    public:

        //--------------------------------------------------
        // iterators
        //--------------------------------------------------
        class SSLL_Iter: public std::iterator<std::forward_iterator_tag, T>
        {
        public:
            // inheriting from std::iterator<std::forward_iterator_tag, T>
            // automagically sets up these typedefs...
            typedef T value_type;
            typedef std::ptrdiff_t difference_type;
            typedef T& reference;
            typedef T* pointer;
            typedef std::forward_iterator_tag iterator_category;

            // but not these typedefs...
            typedef SSLL_Iter self_type;
            typedef SSLL_Iter& self_reference;

        private:
            Node* here;

        public:
            explicit SSLL_Iter(Node* start) : here(start) {
                if (start == nullptr)
                    throw std::runtime_error("SSLL_Iter: start cannot be null");
            }
            SSLL_Iter(const SSLL_Iter& src) : here(src.here) {
                if (*this != src)
                    throw std::runtime_error("SSLL_Iter: copy constructor failed");
            }
            reference operator*() const {
                return here->item;
            }
            pointer operator->() const {
                return & this->operator*();
            }
            self_reference operator=( const self_type& src ) {
                if (&src == this)
                    return *this;
                here = src.here;
                if (*this != src)
                    throw std::runtime_error("SSLL_Iter: copy assignment failed");
                return *this;
            }
            self_reference operator++() { // preincrement
                if (here->next == nullptr)
                    throw std::out_of_range("SSLL_Iter: Can't traverse past the end of the list");
                here = here->next;
                return *this;
            }
            self_type operator++(int) { // postincrement
                self_type t(*this); //save state
                operator++(); //apply increment
                return t; //return state held before increment
            }
            bool operator==(const self_type& rhs) const {
                return rhs.here == here;
            }
            bool operator!=(const self_type& rhs) const {
                return ! operator==(rhs);
            }
        };

        class SSLL_Const_Iter: public std::iterator<std::forward_iterator_tag, T>
        {
        public:
            // inheriting from std::iterator<std::forward_iterator_tag, T>
            // automagically sets up these typedefs...
            typedef T value_type;
            typedef std::ptrdiff_t difference_type;
            typedef const T& reference;
            typedef const T* pointer;
            typedef std::forward_iterator_tag iterator_category;

            // but not these typedefs...
            typedef SSLL_Const_Iter self_type;
            typedef SSLL_Const_Iter& self_reference;

        private:
            const Node* here;

        public:
            explicit SSLL_Const_Iter(Node* start) : here(start) {
                if (start == nullptr)
                    throw std::runtime_error("SSLL_Const_Iter: start cannot be null");
            }
            SSLL_Const_Iter(const SSLL_Const_Iter& src) : here(src.here) {
                if (*this != src)
                    throw std::runtime_error("SSLL_Const_Iter: copy constructor failed");
            }

            reference operator*() const {
                return here->item;
            }
            pointer operator->() const {
                return & this->operator*();
            }
            self_reference operator=( const self_type& src ) {
                if (&src == this)
                    return *this;
                here = src.here;
                if (*this != src)
                    throw std::runtime_error("SSLL_Const_Iter: copy assignment failed");
                return *this;
            }
            self_reference operator++() { // preincrement
                if (here->next == nullptr)
                    throw std::out_of_range("SSLL_Const_Iter: Can't traverse past the end of the list");
                here = here->next;
                return *this;
            }
            self_type operator++(int) { // postincrement
                self_type t(*this); //save state
                operator++(); //apply increment
                return t; //return state held before increment
            }
            bool operator==(const self_type& rhs) const {
                return rhs.here == here;
            }
            bool operator!=(const self_type& rhs) const {
                return ! operator==(rhs);
            }
        };

        //--------------------------------------------------
        // types
        //--------------------------------------------------
        typedef T value_type;
        typedef SSLL_Iter iterator;
        typedef SSLL_Const_Iter const_iterator;

        iterator begin() { return SSLL_Iter(head->next); }
        iterator end() { return SSLL_Iter(tail); }

        const_iterator begin() const { return SSLL_Const_Iter(head->next); }
        const_iterator end() const { return SSLL_Const_Iter(tail); }

        //--------------------------------------------------
        // operators
        //--------------------------------------------------
        T& operator[](size_t i) {
            if (i >= size()) {
                throw std::out_of_range(std::string("operator[]: No element at position ") + std::to_string(i));
            }
            return node_at(i)->item;
        }

        const T& operator[](size_t i) const {
            if (i >= size()) {
                throw std::out_of_range(std::string("operator[]: No element at position ") + std::to_string(i));
            }
            return node_at(i)->item;
        }

        //--------------------------------------------------
        // Constructors/destructor/assignment operator
        //--------------------------------------------------

        SSLL() {
            init();
        }
        //--------------------------------------------------
        //copy constructor
        //note to self: src must be const in case we want to assign this from a const source
        SSLL(const SSLL& src) {
            init();
            copy_constructor(src);
        }

        //--------------------------------------------------
        //destructor
        ~SSLL() {
            // safely dispose of this SSLL's contents
            clear();
        }

        //--------------------------------------------------
        //copy assignment constructor
        SSLL& operator=(const SSLL& src) {
            if (&src == this) // check for self-assignment
                return *this;     // do nothing
            // safely dispose of this SSLL's contents
            // populate this SSLL with copies of the other SSLL's contents
            clear();
            copy_constructor(src);
            return *this;
        }

        //--------------------------------------------------
        // member functions
        //--------------------------------------------------

        /*
            replaces the existing element at the specified position with the specified element and
            returns the original element.
        */
        T replace(const T& element, size_t position) {
            T old_item;
            if (position >= size()) {
                throw std::out_of_range(std::string("replace: No element at position ") + std::to_string(position));
            } else {
                //we are guaranteed to be at a non-dummy item now because of the above if statement
                Node* iter = node_at(position);
                old_item = iter->item;
                iter->item = element;
            }
            return old_item;
        }

        //--------------------------------------------------
        /*
            adds the specified element to the list at the specified position, shifting the element
            originally at that and those in subsequent positions one position to the ”right.“
        */
        void insert(const T& element, size_t position) {
            if (position > size()) {
                throw std::out_of_range(std::string("insert: Position is outside of the list: ") + std::to_string(position));
            } else if (position == size()) {
                //special O(1) case
                push_back(element);
            } else {
                //node_before_position is guaranteed to point to a valid node because we use a dummy head node
                Node* node_before_position = node_before(position);
                Node* node_at_position = node_before_position->next;
                Node* new_node;
                try {
                    new_node = design_new_node(element, node_at_position);
                } catch (std::bad_alloc& ba) {
                    std::cerr << "insert(): failed to allocate memory for new node" << std::endl;
                    throw std::bad_alloc();
                }
                insert_node_after(node_before_position, new_node);
            }
        }

        /*
            prepends the specified element to the list.
        */
        void push_front(const T& element) {
            insert(element, 0);
        }

        //--------------------------------------------------
        /*
            appends the specified element to the list.
        */
        void push_back(const T& element) {
            Node* new_tail;
            try {
                new_tail = design_new_node(nullptr, true);
            } catch (std::bad_alloc& ba) {
                std::cerr << "push_back(): failed to allocate memory for new tail" << std::endl;
                throw std::bad_alloc();
            }
            insert_node_after(tail, new_tail);
            //transform the current tail node from a dummy to a real node holding element
            tail->is_dummy = false;
            tail->item = element;
            tail->next = new_tail;
            tail = tail->next;
        }

        /*
            removes and returns the element at the list's head.
        */
        T pop_front() {
            if (is_empty()) {
                throw std::out_of_range("pop_front: Can't pop: list is empty");
            }
            if (head->next == tail) {
                throw std::runtime_error("pop_front: head->next == tail, but list says it's not empty (corrupt state)");
            }
            return remove_item_after(head);
        }

        //--------------------------------------------------
        /*
            removes and returns the element at the list's tail.
        */
        T pop_back() {
            if (is_empty()) {
                throw std::out_of_range("pop_back: Can't pop: list is empty");
            }
            if (head->next == tail) {
                throw std::runtime_error("pop_back: head->next == tail, but list says it's not empty (corrupt state)");
            }
            //XXX this is O(N), a disadvantage of this architecture
            Node* node_before_last = node_before(size() - 1);
            T item = remove_item_after(node_before_last);
            return item;
        }

        //--------------------------------------------------
        /*
            removes and returns the the element at the specified position,
            shifting the subsequent elements one position to the ”left.“
        */
        T remove(size_t position) {
            T item;
            if (position >= size()) {
                throw std::out_of_range(std::string("remove: No element at position ") + std::to_string(position));
            } 
            if (head->next == tail) {
                throw std::runtime_error("remove: head->next == tail, but list says it's not empty (corrupt state)");
            }
            //using a dummy head node guarantees that there be a node immediately preceeding the specified position
            Node *node_before_position = node_before(position);
            item = remove_item_after(node_before_position);
            return item;
        }

        //--------------------------------------------------
        /*
            returns (without removing from the list) the element at the specified position.
        */
        T item_at(size_t position) const {
            if (position >= size()) {
                throw std::out_of_range(std::string("item_at: No element at position ") + std::to_string(position));
            }
            return operator[](position);
        }

        //--------------------------------------------------
        /*
            returns true IFF the list contains no elements.
        */
        bool is_empty() const {
            return size() == 0;
        }

        //--------------------------------------------------
        /*
            returns the number of elements in the list.
        */
        size_t size() const {
            if (num_items == 0 && head->next != tail) {
                throw std::runtime_error("size: head->next != tail, but list says it's empty (corrupt state)");
            } else if (num_items > 0 && head->next == tail) {
                throw std::runtime_error("size: head->next == tail, but list says it's not empty (corrupt state)");
            }
            return num_items;
        }

        //--------------------------------------------------
        /*
            removes all elements from the list.
        */
        void clear() {
            while ( ! is_empty()) {
                pop_front();
            }
        }

        //--------------------------------------------------
        /*
            returns true IFF one of the elements of the list matches the specified element.
        */
        bool contains(const T& element,
               bool equals(const T& a, const T& b)) const {
            bool element_in_list = false;
            const_iterator fin = end();
            for (const_iterator iter = begin(); iter != fin; ++iter) {
                if (equals(*iter, element)) {
                    element_in_list = true;
                    break;
                }
            }
            return element_in_list;
        }

        //--------------------------------------------------
        /*
            If the list is empty, inserts "<empty list>" into the ostream;
            otherwise, inserts, enclosed in square brackets, the list's elements,
            separated by commas, in sequential order.
        */
        std::ostream& print(std::ostream& out) const {
            if (is_empty()) {
                out << "<empty list>";
            } else {
                out << "[";
                const_iterator start = begin();
                const_iterator fin = end();
                for (const_iterator iter = start; iter != fin; ++iter) {
                    if (iter != start)
                        out << ",";
                    out << *iter;
                }
                out << "]";
            }
            return out;
        }
    protected:
        bool validate_internal_integrity() {
            //todo: fill this in
            return true;
        }
    }; //end class SSLL
} // end namespace cop3530
#endif // _SSLL_H_
