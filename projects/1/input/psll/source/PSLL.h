#ifndef _PSLL_H_
#define _PSLL_H_

// PSLL.H
//
// Pool-using Singly-linked list (non-polymorphic)
//
// Authors: Paul Nickerson, Dave Small
// for COP 3530
// 201409.16 - created

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <string>

namespace cop3530 {
    template <class T>
    class PSLL {
    private:
        struct Node {
            T item;
            Node* next;
            bool is_dummy;
        }; // end struct Node
        size_t num_main_list_items;
        size_t num_free_list_items;
        Node* head;
        Node* tail;
        Node* free_list_head;
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
        Node* procure_free_node(bool force_allocation) {
            Node* n;
            if (force_allocation || free_list_size() == 0) {
                n = new Node();
            } else {
                n = remove_node_after(free_list_head, num_free_list_items);
            }
            return n;
        }
        void shrink_pool_if_necessary() {
            if (size() >= 100) {
                while (free_list_size() > size() / 2) { //while the pool contains more nodes than half the list size
                    Node* n = remove_node_after(free_list_head, num_free_list_items);
                    delete n;
                }
            }
        }

        size_t free_list_size() { return num_free_list_items; }
        Node* design_new_node(const T& element, Node* next = nullptr, bool dummy = false, bool force_allocation = false) {
            Node* n = procure_free_node(force_allocation);
            n->is_dummy = dummy;
            n->item = element;
            n->next = next;
            return n;
        }
        Node* design_new_node(Node* next = nullptr, bool dummy = false, bool force_allocation = false) {
            Node* n = procure_free_node(force_allocation);
            n->is_dummy = dummy;
            n->next = next;
            return n;
        }
        void init() {
            num_main_list_items = 0;
            num_free_list_items = 0;
            free_list_head = design_new_node(nullptr, true, true);
            tail = design_new_node(nullptr, true, true);
            head = design_new_node(tail, true, true);
        }
        void copy_constructor(const PSLL& src) {
            //note: this function does *not* copy the free list
            const_iterator fin = src.end();
            for (const_iterator iter = src.begin(); iter != fin; ++iter) {
                push_back(*iter);
            }
        }
        Node* remove_node_after(Node* preceeding_node, size_t& list_size_counter) {
            assert(preceeding_node->next != tail);
            assert(preceeding_node != tail);
            assert( ! (preceeding_node == free_list_head && free_list_size() == 0));
            Node* removed_node = preceeding_node->next;
            preceeding_node->next = removed_node->next;
            removed_node->next = nullptr;
            --list_size_counter;
            return removed_node;
        }

        void insert_node_after(Node* existing_node, Node* new_node, size_t& list_size_counter) {
            new_node->next = existing_node->next;
            existing_node->next = new_node;
            ++list_size_counter;
        }

        //returns subsequent node's item and moves that node to the free pool
        T remove_item_after(Node* preceeding_node) {
            Node* removed_node = remove_node_after(preceeding_node, num_main_list_items);
            T item = removed_node->item;
            insert_node_after(free_list_head, removed_node, num_free_list_items);
            shrink_pool_if_necessary();
            return item;
        }

    public:
        //--------------------------------------------------
        // iterators
        //--------------------------------------------------
        class PSLL_Iter: public std::iterator<std::forward_iterator_tag, T> {
        private:
            Node* here;
        public:
            typedef T value_type;
            typedef std::ptrdiff_t difference_type;
            typedef T* pointer;
            typedef T& reference;
            typedef std::forward_iterator_tag iterator_category;

            typedef PSLL_Iter self_type;
            typedef PSLL_Iter& self_reference;

            explicit PSLL_Iter(Node* start): here(start) {
                if (start == nullptr)
                    throw std::runtime_error("PSLL_Iter: start cannot be null");
            }
            PSLL_Iter(const self_type& src): here(src.here) {}

            reference operator*() const {
                return here->item;
            }
            pointer operator->() const {
                return & this->operator*();
            }
            self_reference operator=(const self_type& src) {
                //copy assigner
                if (&src == this)
                    return *this;
                here = src.here;
                return *this;
            }
            self_reference operator++() {
                //prefix
                here = here->next;
                return *this;
            }
            self_type operator++(int) {
                self_type t(*this); //save state
                operator++(); //apply increment
                return t; //return state held before increment
            }
            bool operator==(const self_type& rhs) const {
                return here == rhs.here;
            }
            bool operator!=(const self_type& rhs) const {
                return ! operator==(rhs);
            }
        };

        class PSLL_Const_Iter: public std::iterator<std::forward_iterator_tag, T> {
        private:
            const Node* here;
        public:
            typedef T value_type;
            typedef std::ptrdiff_t difference_type;
            typedef const T* pointer;
            typedef const T& reference;
            typedef std::forward_iterator_tag iterator_category;

            typedef PSLL_Const_Iter self_type;
            typedef PSLL_Const_Iter& self_reference;

            explicit PSLL_Const_Iter(Node* start): here(start) {
                if (start == nullptr)
                    throw std::runtime_error("PSLL_Const_Iter: start cannot be null");
            }
            PSLL_Const_Iter(const self_type& src): here(src.here) {}

            reference operator*() const {
                return here->item;
            }
            pointer operator->() const {
                return & this->operator*();
            }
            self_reference operator=(const self_type& src) {
                //copy assigner
                if (&src == this)
                    return *this;
                here = src.here;
                return *this;
            }
            self_reference operator++() {
                //prefix
                here = here->next;
                return *this;
            }
            self_type operator++(int) {
                self_type t(*this); //save state
                operator++(); //apply increment
                return t; //return state held before increment
            }
            bool operator==(const self_type& rhs) const {
                return here == rhs.here;
            }
            bool operator!=(const self_type& rhs) const {
                return ! operator==(rhs);
            }
        };

        //--------------------------------------------------
        // types
        //--------------------------------------------------
        /*typedef std::size_t size_t;*/
        typedef T value_type;
        typedef PSLL_Iter iterator;
        typedef PSLL_Const_Iter const_iterator;

        iterator begin() {
            return iterator(head->next);
        }
        iterator end() {
            return iterator(tail);
        }
        /*
            Note to self: the following overloads will fail if not defined as const
        */
        const_iterator begin() const {
            return const_iterator(head->next);
        }
        const_iterator end() const {
            return const_iterator(tail);
        }

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

        PSLL() {
            init();
        }
        //--------------------------------------------------
        //copy constructor
        PSLL(const PSLL& src) {
            init();
            copy_constructor(src);
        }

        //--------------------------------------------------
        //destructor
        ~PSLL() {
            // safely dispose of this PSLL's contents
            clear();
        }

        //--------------------------------------------------
        //copy assignment constructor
        PSLL& operator=(const PSLL& src) {
            if (&src == this) // check for self-assignment
                return *this;     // do nothing
            // safely dispose of this PSLL's contents
            // populate this PSLL with copies of the other PSLL's contents
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
            } else {
                //node_before_position is guaranteed to point to a valid node because we use a dummy head node
                Node* node_before_position = node_before(position);
                Node* node_at_position = node_before_position->next;
                Node* new_node = design_new_node(element, node_at_position);
                insert_node_after(node_before_position, new_node, num_main_list_items);
            }
        }

        //--------------------------------------------------
        //Note to self: use reference here because we receive the original object instance,
        //then copy it into n->item so we have it if the original element goes out of scope
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
            Node* new_tail = design_new_node(nullptr, true);
            insert_node_after(tail, new_tail, num_main_list_items);
            //transform the current tail node from a dummy to a real node holding element
            tail->is_dummy = false;
            tail->item = element;
            tail->next = new_tail;
            tail = tail->next;
        }

        //--------------------------------------------------
        //Note to self: no reference here, so we get our copy of the item, then return a copy
        //of that so the client still has a valid instance if our destructor is called
        /*
            removes and returns the element at the list's head.
        */
        T pop_front() {
            if (is_empty()) {
                throw std::out_of_range("pop_front: Can't pop: list is empty");
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
            } else {
                //using a dummy head node guarantees that there be a node immediately preceeding the specified position
                Node *node_before_position = node_before(position);
                item = remove_item_after(node_before_position);
            }
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
            return node_at(position)->item;
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
            assert( ! (num_main_list_items == 0 && head->next != tail));
            return num_main_list_items;
        }

        //--------------------------------------------------
        /*
            removes all elements from the list.
        */
        void clear() {
            while (size()) {
                remove_item_after(head);
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
    }; //end class PSLL
} // end namespace cop3530
#endif // _PSLL_H_
