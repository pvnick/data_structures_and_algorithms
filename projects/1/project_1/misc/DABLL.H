#ifndef _DABLL_H_
#define _DABLL_H_

// DABLL.H
//
// Singly-linked list (non-polymorphic)
//
// by Dave Small and Paul Nickerson
// for COP 3530
// 201409.16 - created

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <string>
#include <cmath>

namespace cop3530 {
    template <class T>
    class ListFuzzer;

    template <class T>
    class DABLL {
    friend class ListFuzzer<T>;
    private:
        struct Node {
            bool is_dummy;
            Node* next;
            T item;
        }; // end struct Node
        class ListState {
        private:
            Node* node_array;
            std::allocator<Node> alloc;
            Node* free_list_head;
        public:
        //XXX: some of the following functions should be private
            int num_filled_slots;
            int num_unfilled_slots;
            static int num_dummy_nodes; //head, tail, free
            Node* head;
            Node* tail;

            ListState(int client_slots_to_preallocate = 50) {
                if (client_slots_to_preallocate <= 0) {
                    throw std::runtime_error("must preallocate more than 0 nodes");
                }
                //since dummy nodes take up space in the allocated memory, allocate some extra space for them
                num_unfilled_slots = client_slots_to_preallocate;
                num_filled_slots = num_dummy_nodes;
                node_array = allocate_sequential_nodes(client_slots_to_preallocate + num_dummy_nodes);
                free_list_head = node_array;
                free_list_head->is_dummy = true;
                free_list_head->next = node_array + 3;
                head = node_array + 1;
                head->is_dummy = true;
                head->next = tail = node_array + 2;
                tail->is_dummy = true;
                tail->next = nullptr;
                /*
                allocated memory should now look like this:
                       __________________________________
                      |                                  v
                -----------------------------------------------------
                | free head (0) | head (1) | tail (2) |      |      | ...
                -----------------------------------------------------
                                        |_____^    |
                                                   v
                                                nullptr
                */
                assert(free_list_head->next == tail + 1);
            }
            void destroy_all_nodes() {
                for (unsigned int node_index = 0; node_index < num_filled_slots + num_unfilled_slots; ++node_index)
                    alloc.destroy(node_array + node_index);
                alloc.deallocate(node_array, num_filled_slots + num_unfilled_slots);
            }
            Node* node_at(int position) const {
                Node* iter = head;
                for (int i = -1; i != position; ++i, iter = iter->next);
                return iter;
            }
            void insert_node_after(Node* existing_node, Node* new_node, int& list_size_counter_to_increment) const {
                new_node->next = existing_node->next;
                existing_node->next = new_node;
                ++list_size_counter_to_increment;
            }
            //returns subsequent node's item and deletes its slot
            T remove_item_after(Node* preceeding_node) {
                Node* removed_node = remove_node_after(preceeding_node, num_filled_slots);
                T item = removed_node->item;
                insert_node_after(free_list_head, removed_node, num_unfilled_slots);
                shrink_if_necessary();
                return item;
            }
            Node* allocate_sequential_nodes(int num_nodes) {
                Node* nodes = alloc.allocate(num_nodes);
                Node n;
                std::uninitialized_fill_n(nodes, num_nodes, n);
                for (int i = 0; i != num_nodes; ++i) {
                    if (i == num_nodes - 1) {
                        nodes[i].next = nullptr;
                    } else {
                        nodes[i].next = nodes + i + 1;
                    }
                }
                return nodes;
            }
            Node* procure_free_node() {
                embiggen_if_necessary();
                //note: this decrements the num_unfilled_slots counter
                Node* n;
                n = remove_node_after(free_list_head, num_unfilled_slots);
                return n;
            }
            //it is crucial that, when designing a new node, it is inserted somewhere in the list
            //otherwise the filled/unfilled slot counter will get screwed up
            Node* design_new_node(const T& element, Node* next = nullptr, bool dummy = false) {
                Node* n = procure_free_node();
                n->is_dummy = dummy;
                n->item = element;
                n->next = next;
                return n;
            }
            Node* design_new_node(Node* next = nullptr, bool dummy = false) {
                Node* n = procure_free_node();
                n->is_dummy = dummy;
                n->next = next;
                return n;
            }
            Node* remove_node_after(Node* preceeding_node, int& list_size_counter_to_decrement) {
                assert(preceeding_node->next != tail);
                assert(preceeding_node != tail);
                assert( ! (preceeding_node == free_list_head && num_unfilled_slots == 0));
                Node* removed_node = preceeding_node->next;
                preceeding_node->next = removed_node->next;
                removed_node->next = nullptr;
                --list_size_counter_to_decrement;
                return removed_node;
            }
            void copy_state_filled_slots(const ListState& src) {
                Node *insertion_point = head;
                for (Node* iter = src.head->next; iter != src.tail; iter = iter->next) {
                    Node* copied_node = design_new_node(iter->item);
                    insert_node_after(insertion_point, copied_node, num_filled_slots);
                    insertion_point = copied_node;
                }
            }
            void embiggen_if_necessary() {
                /*
                Whenever an item is added and the backing array is full, allocate a new array 150% the size
                of the original, copy the items over to the new array, and deallocate the original one.
                */
                if (num_unfilled_slots == 0) {
                    int curr_client_slots = num_filled_client_nodes(); //excludes dummy nodes
                    int new_client_slots = ceil(curr_client_slots * 1.5);
                    ListState s(new_client_slots);
                    s.copy_state_filled_slots(*this);
                    destroy_all_nodes();
                    *this = s;
                }
            }
            void shrink_if_necessary() {
                /*
                Because we don't want the list to waste too much memory, whenever the array's size is ≥ 100
                slots and fewer than half the slots are used, allocate a new array 50% the size of the original,
                copy the items over to the new array, and deallocate the original one.
                */
                if (num_unfilled_slots + num_filled_client_nodes() >= 100
                    && num_filled_client_nodes() < num_unfilled_slots)
                {
                    int curr_client_slots = num_unfilled_slots + num_filled_client_nodes(); //excludes dummy nodes
                    int new_client_slots = ceil(curr_client_slots * 0.5);
                    ListState s(new_client_slots);
                    s.copy_state_filled_slots(*this);
                    destroy_all_nodes();
                    *this = s;
                }
            }
            int num_filled_client_nodes() const {
                //since dummy nodes constitute filled slots, subtract those to get the number of client-filled slots
                assert( ! (num_filled_slots == num_dummy_nodes && head->next != tail));
                return num_filled_slots - num_dummy_nodes;
            }
        };

        ListState my_state;
        const std::string get_class_name() const {
            return "DABLL";
        }
        bool validate_internal_integrity() {
            //todo: fill this in
            return true;
        }
        void copy_constructor(const DABLL& src) {
            ListState new_state(src.my_state.num_filled_slots + src.my_state.num_unfilled_slots - src.my_state.num_dummy_nodes);
            new_state.copy_state_filled_slots(src.my_state);
            my_state.destroy_all_nodes();
            my_state = new_state;
        }

    public:
        //--------------------------------------------------
        // types
        //--------------------------------------------------
        /*typedef std::size_t size_t;
        typedef T value_type;
        typedef XXX_Iter iterator;
        typedef XXX_Const_Iter const_iterator;
*/
        //--------------------------------------------------
        // Constructors/destructor/assignment operator
        //--------------------------------------------------

        DABLL(int num_nodes_to_preallocate = 50): my_state(num_nodes_to_preallocate) {}
        //--------------------------------------------------
        //copy constructor
        DABLL(const DABLL& src) {
            copy_constructor(src);
        }

        //--------------------------------------------------
        //destructor
        ~DABLL() {
            // safely dispose of this DABLL's contents
            my_state.destroy_all_nodes();
        }

        //--------------------------------------------------
        //copy assignment constructor
        DABLL& operator=(const DABLL& src) {
            if (&src == this) // check for self-assignment
                return *this;     // do nothing
            // populate this DABLL with copies of the other DABLL's contents
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
        T replace(const T& element, int position) {
            T old_item;
            if (position >= size()) {
                throw std::runtime_error(std::string("No element at position ") + std::to_string(position));
            } else {
                //we are guaranteed to be at a non-dummy item now because of the above if statement
                Node* iter = my_state.node_at(position);
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
        void insert(const T& element, int position) {
            if (position > size()) {
                throw std::runtime_error(std::string("Position is outside of the list: ") + std::to_string(position));
            } else {
                //node_before_position is guaranteed to point to a valid node because we use a dummy head node
                Node* new_node = my_state.design_new_node(element); //this function must run first because our node pointers will change if we resize
                Node* node_before_position = my_state.node_at(position - 1);
                my_state.insert_node_after(node_before_position, new_node, my_state.num_filled_slots);
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
            Node* new_tail = my_state.design_new_node(nullptr, true);
            my_state.insert_node_after(my_state.tail, new_tail, my_state.num_filled_slots);
            //transform the current tail node from a dummy to a real node holding element
            Node* n = my_state.tail;
            n->is_dummy = false;
            n->item = element;
            n->next = new_tail;
            my_state.tail = new_tail;
            assert(n->next == my_state.tail);
        }


        //--------------------------------------------------
        //Note to self: no reference here, so we get our copy of the item, then return a copy
        //of that so the client still has a valid instance if our destructor is called
        /*
            removes and returns the element at the list's head.
        */
        T pop_front() {
            if (is_empty()) {
                throw std::runtime_error("Can't pop: list is empty");
            }
            return my_state.remove_item_after(my_state.head);
        }

        //--------------------------------------------------
        /*
            removes and returns the element at the list's tail.
        */
        T pop_back() {
            if (is_empty()) {
                throw std::runtime_error("Can't pop: list is empty");
            }
            //XXX this is O(N), a disadvantage of this architecture
            Node* node_before_last = my_state.node_at(size() - 2);
            T item = my_state.remove_item_after(node_before_last);
            return item;
        }

        //--------------------------------------------------
        /*
            removes and returns the the element at the specified position,
            shifting the subsequent elements one position to the ”left.“
        */
        T remove(int position) {
            T item;
            if (position >= size()) {
                throw std::runtime_error(std::string("No element at position ") + std::to_string(position));
            } else {
                //using a dummy head node guarantees that there be a node immediately preceeding the specified position
                Node *node_before_position = my_state.node_at(position - 1);
                item = my_state.remove_item_after(node_before_position);
            }
            return item;
        }

        //--------------------------------------------------
        /*
            returns (without removing from the list) the element at the specified position.
        */
        T item_at(int position) const {
            T item;
            if (position >= size()) {
                throw std::runtime_error(std::string("No element at position ") + std::to_string(position));
            } else {
                Node* node_at_position = my_state.node_at(position);
                item = node_at_position->item;
            }
            return item;
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
        int size() const {
            return my_state.num_filled_client_nodes();
        }

        //--------------------------------------------------
        /*
            removes all elements from the list.
        */
        void clear() {
            while (size()) {
                my_state.remove_item_after(my_state.head);
            }
        }

        //--------------------------------------------------
        /*
            returns true IFF one of the elements of the list matches the specified element.
        */
        bool contains(const T& element,
               bool equals(const T& a, const T& b)) const {
            bool element_in_list = false;
            for (Node* iter = my_state.head->next; iter != my_state.tail; iter = iter->next) {
                if (equals(iter->item, element)) {
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
                for (Node* iter = my_state.head->next; iter != my_state.tail; iter = iter->next) {
                    out << iter->item;
                    if (iter->next != my_state.tail) {
                        out << ",";
                    }
                }
                out << "]";
            }
            return out;
        }
    };
} // end namespace cop3530

template<typename T>
int cop3530::DABLL<T>::ListState::num_dummy_nodes = 3;

#endif // _DABLL_H_
