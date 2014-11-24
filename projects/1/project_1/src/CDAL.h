#ifndef _CDAL_H_
#define _CDAL_H_

// CDAL.H
//
// Chained Dynamic Array-based List (non-polymorphic)
//
// Authors: Paul Nickerson, Dave Small
// for COP 3530
// 201409.16 - created

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <math.h>

namespace cop3530 {
    template <class T>
    class ListTester;

    template <class T>
    class CDAL {
    friend class ListTester<T>;
    private:
        struct Node {
            //Node is an element in the linked list and contains an array of items
            T* item_array;
            Node* next;
            bool is_dummy;
        };
        struct ItemLoc {
            //ItemLoc describes the position of an item, including its linked list node and position within the array held by that node
            Node* node;
            size_t array_index;
            T& item_ref;
        };
        size_t num_items;
        size_t num_available_nodes; //excludes head/tail nodes
        size_t embiggen_counter = 0;
        size_t shrink_counter = 0;
        Node* head;
        Node* tail;
        static const size_t array_size = 50; //length of each chained array
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

        ItemLoc loc_from_pos(size_t position) const {
            size_t node_position = floor(position / array_size);
            Node* n = node_at(node_position);
            size_t array_index = position % array_size;
            ItemLoc loc {n, array_index, n->item_array[array_index]};
            return loc;
        }

        Node* design_new_node(Node* next = nullptr, bool dummy = false) const {
            Node* n = new Node();
            n->is_dummy = dummy;
            n->item_array = new T[array_size];
            n->next = next;
            return n;
        }

        void init() {
            num_items = 0;
            num_available_nodes = 0;
            tail = design_new_node(nullptr, true);
            head = design_new_node(tail, true);
        }

        void free_node(Node* n) {
            delete[] n->item_array;
            delete n;
        }

        void drop_node_after(Node* n) {
            assert(n->next != tail);
            Node* removed_node = n->next;
            n->next = removed_node->next;
            free_node(removed_node);
            --num_available_nodes;
        }

        size_t num_used_nodes() {
            return ceil(size() / array_size);
        }

        void embiggen_if_necessary() {
            //embiggen is a perfectly cromulent word
            if (size() == num_available_nodes * array_size) {
                //transform tail into a regular node and append a new tail
                Node* n = tail;
                n->is_dummy = false;
                tail = n->next = design_new_node(nullptr, false);
                ++num_available_nodes;
                ++embiggen_counter;
            }
        }

        void shrink_if_necessary() {
            /*
                Because we don't want the list to waste too much memory, whenever the more than half of the arrays
                are unused (they would all be at the end of the chain), deallocate half the unused arrays.
            */
            size_t used = num_used_nodes();
            size_t num_unused_nodes = num_available_nodes - used;
            if (num_unused_nodes > used) {
                size_t nodes_to_keep = used + ceil(num_unused_nodes * 0.5);
                Node* last_node = node_before(nodes_to_keep);
                while (last_node->next != tail) {
                    drop_node_after(last_node);
                }
                ++shrink_counter;
            }
        }

        void copy_constructor(const CDAL& src) {
            assert(num_available_nodes <= src.num_available_nodes);
            //allocate all the nodes we need
            for (size_t i = 0; i != src.num_available_nodes; ++i) {
                Node* n = design_new_node(head->next, false);
                head->next = n;
                ++num_available_nodes;
            }
            Node* dst_n = head;
            Node* src_n = src.head;
            for (size_t i = 0; i != src.num_items; ++i) {
                size_t array_index = i % array_size;
                if (array_index == 0) {
                    dst_n = dst_n->next;
                    src_n = src_n->next;
                }
                dst_n->item_array[array_index] = src_n->item_array[array_index];
            }
            num_items = src.num_items;
        }

    public:
        //--------------------------------------------------
        // iterators
        //--------------------------------------------------
        class CDAL_Iter: public std::iterator<std::forward_iterator_tag, T> {
        private:
            Node* here_container;
            size_t here_index;
        public:
            typedef std::ptrdiff_t difference_type;
            typedef T& reference;
            typedef T* pointer;
            typedef std::forward_iterator_tag iterator_category;
            typedef T value_type;
            typedef CDAL_Iter self_type;
            typedef CDAL_Iter& self_reference;

            //need copy constructor/assigner to make this a first class ADT (doesn't hold pointers that need freeing)
            CDAL_Iter(Node* container, size_t index): here_container(container), here_index(index) {}
            CDAL_Iter(const self_type& src): here_container(src.here_container), here_index(src.here_index) {}
            //can use self-typedefs anywhere but the constructor
            self_reference operator=(const self_type& rhs) {
                //copy assigner
                if (&rhs == this) return *this;
                here_container = rhs.here_container;
                here_index = rhs.here_index;
                return this;
            }
            self_reference operator++() {
                //prefix (no int parameter)
                here_index = (here_index + 1) % array_size;
                if (here_index == 0) here_container = here_container->next;
                return *this;
            }
            self_reference operator++(int) {
                self_type t(*this); //save state
                operator++(); //apply increment
                return t; //return state held before increment
            }
            reference operator*() {
                return here_container->item_array[here_index];
            }
            pointer operator->() {
                return & this->operator*();
            }
            bool operator==(const self_type& rhs) {
                return rhs.here_index == here_index
                        && rhs.here_container == here_container;
            }
            bool operator!=(const self_type& rhs) {
                return ! operator==(rhs);
            }
        };

        class CDAL_Const_Iter: public std::iterator<std::forward_iterator_tag, T> {
        private:
            const Node* here_container;
            size_t here_index;
        public:
            //todo: check on whether value_type should/shouldn't be const
            typedef const T value_type;
            typedef const T& reference;
            typedef const T* pointer;
            typedef std::forward_iterator_tag iterator_category;
            //todo: check on the meaning of the following statement
            typedef std::ptrdiff_t difference_type;
            typedef CDAL_Const_Iter self_type;
            typedef CDAL_Const_Iter& self_reference;

            CDAL_Const_Iter(const Node* container, size_t index): here_container(container), here_index(index) {}
            CDAL_Const_Iter(const self_type& src): here_container(src.here_container), here_index(src.here_index) {}
            self_reference operator=(const self_type& rhs) {
                if (&rhs == this) return *this;
                here_container = rhs.here_container;
                here_index = rhs.here_index;
                return *this;
            }
            self_reference operator++() {
                here_index = (here_index + 1) % array_size;
                if (here_index == 0) here_container = here_container->next;
                return *this;
            }
            self_reference operator++(int) {
                self_type t(*this); //save state
                operator++(); //apply increment
                return t; //return state held before increment
            }
            bool operator==(const self_type& rhs) {
                return rhs.here_container == here_container
                        && rhs.here_index == here_index;
            }
            bool operator!=(const self_type& rhs) {
                return ! operator==(rhs);
            }
            reference operator*() {
                return here_container->item_array[here_index];
            }
            pointer operator->() {
                return & this->operator*();
            }
        };

        //--------------------------------------------------
        // types
        //--------------------------------------------------
        typedef CDAL_Iter iterator;
        typedef CDAL_Const_Iter const_iterator;
        typedef T value_type;
        //todo: might need to add size_t here and other iterators if they were excluded or commented out

        iterator begin() {
            return iterator(head->next, 0);
        }

        iterator end() {
            ItemLoc end_loc = loc_from_pos(size());
            return iterator(end_loc.node, end_loc.array_index);
        }

        const_iterator begin() const {
            return const_iterator(head->next, 0);
        }

        const_iterator end() const {
            ItemLoc end_loc = loc_from_pos(size());
            return const_iterator(end_loc.node, end_loc.array_index);
        }

        T& operator[](size_t i) {
            if (i >= size()) {
                throw std::out_of_range(std::string("operator[]: No element at position ") + std::to_string(i));
            }
            return loc_from_pos(i).item_ref;
        }

        const T& operator[](size_t i) const {
            if (i >= size()) {
                throw std::out_of_range(std::string("operator[]: No element at position ") + std::to_string(i));
            }
            return loc_from_pos(i).item_ref;
        }

        //--------------------------------------------------
        // Constructors/destructor/assignment operator
        //--------------------------------------------------

        CDAL() {
            init();
            embiggen_if_necessary();
        }
        //--------------------------------------------------
        //copy constructor
        CDAL(const CDAL& src) {
            init();
            copy_constructor(src);
        }

        //--------------------------------------------------
        //destructor
        ~CDAL() {
            // safely dispose of this CDAL's contents
            clear();
        }

        //--------------------------------------------------
        //copy assignment constructor
        CDAL& operator=(const CDAL& src) {
            if (&src == this) // check for self-assignment
                return *this;     // do nothing
            // safely dispose of this CDAL's contents
            // populate this CDAL with copies of the other CDAL's contents
            clear();
            init();
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
            T item = element;
            if (position >= size()) {
                throw std::out_of_range(std::string("replace: No element at position ") + std::to_string(position));
            } else {
                using std::swap;
                ItemLoc loc = loc_from_pos(position);
                swap(loc.item_ref, item);
            }
            return item;
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
                using std::swap;
                embiggen_if_necessary();
                ItemLoc loc = loc_from_pos(position);
                //shift remaining items to the right
                T item_to_insert = element;
                Node* n = loc.node;
                for (size_t i = position; i <= num_items; ++i) {
                    size_t array_index = i % array_size;
                    if ( i != position && array_index == 0 ) {
                        n = n->next;
                    }
                    swap(item_to_insert, n->item_array[array_index]);
                }
                ++num_items;
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
            insert(element, size());
        }

        //--------------------------------------------------
        //Note to self: no reference here, so we get our copy of the item, then return a copy
        //of that so the client still has a valid instance if our destructor is called
        /*
            removes and returns the element at the list's head.
        */
        T pop_front() {
            if (is_empty()) {
                throw std::logic_error("pop_front: Can't pop: list is empty");
            }
            return remove(0);
        }

        //--------------------------------------------------
        /*
            removes and returns the element at the list's tail.
        */
        T pop_back() {
            if (is_empty()) {
                throw std::logic_error("pop_back: Can't pop: list is empty");
            }
            return remove(size() - 1);
        }

        //--------------------------------------------------
        /*
            removes and returns the the element at the specified position,
            shifting the subsequent elements one position to the ”left.“
        */
        T remove(size_t position) {
            T old_item;
            if (position >= size()) {
                throw std::out_of_range(std::string("remove: No element at position ") + std::to_string(position));
            } else {
                using std::swap;
                ItemLoc loc = loc_from_pos(position);
                //shift remaining items to the left
                Node* n = loc.node;
                old_item = loc.item_ref;
                for (size_t i = position; i != num_items; ++i) {
                    size_t curr_array_index = i % array_size;
                    size_t next_array_index = (i + 1) % array_size;
                    T& curr_item = n->item_array[curr_array_index];
                    if ( next_array_index == 0 ) {
                        n = n->next;
                    }
                    T& next_item = n->item_array[next_array_index];
                    swap(curr_item, next_item);
                }
                --num_items;
                shrink_if_necessary();
            }
            return old_item;
        }

        //--------------------------------------------------
        /*
            returns (without removing from the list) the element at the specified position.
        */
        T item_at(size_t position) const {
            if (position >= size()) {
                throw std::out_of_range(std::string("item_at: No element at position ") + std::to_string(position));
            }
            return loc_from_pos(position).item_ref;
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
            return num_items;
        }

        //--------------------------------------------------
        /*
            removes all elements from the list.
        */
        void clear() {
            while (head->next != tail) {
                drop_node_after(head);
            }
            num_items = 0;
        }

        //--------------------------------------------------
        /*
            returns true IFF one of the elements of the list matches the specified element.
        */
        bool contains(const T& element,
               bool equals(const T& a, const T& b)) const {
            bool element_in_list = false;
            Node* n = head;
            for (size_t i = 0; i != num_items; ++i) {
                size_t array_index = i % array_size;
                if (array_index == 0) {
                    n = n->next;
                }
                if (equals(n->item_array[array_index], element)) {
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
            if (size() == 0) {
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
    }; //end class CDAL
} // end namespace cop3530
#endif // _CDAL_H_

