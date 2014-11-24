#ifndef _SDAL_H_
#define _SDAL_H_

// SDAL.H
//
// Singly-linked list (non-polymorphic)
//
// Authors: Paul Nickerson, Dave Small
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
    class ListTester;

    template <class T>
    class SDAL {
    friend class ListTester<T>;
    private:
        T* item_array;
        //XXX: do these both need to be size_t?
        size_t array_size;
        size_t num_items;
        size_t embiggen_counter = 0;
        size_t shrink_counter = 0;
        void embiggen_if_necessary() {
            /*
                 Whenever an item is added and the backing array is full, allocate a new array 150% the size
                 of the original, copy the items over to the new array, and deallocate the original one.
            */
            size_t filled_slots = size();
            if (filled_slots == array_size) {
                size_t new_array_size = ceil(array_size * 1.5);
                T* new_item_array = new T[new_array_size];
                for (size_t i = 0; i != filled_slots; ++i) {
                    new_item_array[i] = item_array[i];
                }
                delete[] item_array;
                item_array = new_item_array;
                array_size = new_array_size;
                ++embiggen_counter;
            }
        }

        void shrink_if_necessary() {
            /*
                Because we don't want the list to waste too much memory, whenever the array's size is ≥ 100 slots
                and fewer than half the slots are used, allocate a new array 50% the size of the original, copy
                the items over to the new array, and deallocate the original one.
            */
            size_t filled_slots = size();
            if (array_size >= 100 && filled_slots < array_size / 2) {
                size_t new_array_size = ceil(array_size * 0.5);
                T* new_item_array = new T[new_array_size];
                for (size_t i = 0; i != filled_slots; ++i) {
                    new_item_array[i] = item_array[i];
                }
                delete[] item_array;
                item_array = new_item_array;
                array_size = new_array_size;
                ++shrink_counter;
            }
        }
    public:

        //--------------------------------------------------
        // iterators
        //--------------------------------------------------
        class SDAL_Iter: public std::iterator<std::forward_iterator_tag, T>
        {
        public:
            // inheriting from std::iterator<std::forward_iterator_tag, T>
            // automagically sets up these typedefs...
            //todo: figure out why we cant comment these out, which we should be able to if they were
            //defined when inheriting
            typedef T value_type;
            typedef std::ptrdiff_t difference_type;
            typedef T& reference;
            typedef T* pointer;
            typedef std::forward_iterator_tag iterator_category;

            // but not these typedefs...
            typedef SDAL_Iter self_type;
            typedef SDAL_Iter& self_reference;

        private:
            T* iter;

        public:
            //todo: solidify understanding of explicit keyword
            explicit SDAL_Iter(T* item_array) : iter(item_array) {}
            SDAL_Iter(const SDAL_Iter& src) : iter(src.iter) {}

            reference operator*() const {
                return *iter;
            }
            pointer operator->() const {
                return & this->operator*();
            }
            self_reference operator=( const self_type& src ) {
                if (&src == this)
                    return *this;
                iter = src.iter;
                return *this;
            }
            self_reference operator++() { // preincrement
                ++iter;
                return *this;
            }
            self_type operator++(int) { // postincrement
                self_type t(*this); //save state
                operator++(); //apply increment
                return t; //return state held before increment
            }
            bool operator==(const self_type& rhs) const {
                return rhs.iter == iter;
            }
            bool operator!=(const self_type& rhs) const {
                return ! operator==(rhs);
            }
        };

        class SDAL_Const_Iter: public std::iterator<std::forward_iterator_tag, T>
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
            typedef SDAL_Const_Iter self_type;
            typedef SDAL_Const_Iter& self_reference;

        private:
            const T* iter;

        public:
            //todo: solidify understanding of explicit keyword
            explicit SDAL_Const_Iter(T* item_array) : iter(item_array) {}
            SDAL_Const_Iter(const SDAL_Const_Iter& src) : iter(src.iter) {}

            reference operator*() const {
                return *iter;
            }
            pointer operator->() const {
                return & this->operator*();
            }
            self_reference operator=( const self_type& src ) {
                if (&src == this)
                    return *this;
                iter = src.iter;
                return *this;
            }
            self_reference operator++() { // preincrement
                ++iter;
                return *this;
            }
            self_type operator++(int) { // postincrement
                self_type t(*this); //save state
                operator++(); //apply increment
                return t; //return state held before increment
            }
            bool operator==(const self_type& rhs) const {
                return rhs.iter == iter;
            }
            bool operator!=(const self_type& rhs) const {
                return ! operator==(rhs);
            }
        };

        //--------------------------------------------------
        // types
        //--------------------------------------------------
        typedef T value_type;
        //note to self: defining these typedefs as public allows us to use their private members
        typedef SDAL_Iter iterator;
        typedef SDAL_Const_Iter const_iterator;

        iterator begin() { return SDAL_Iter(item_array); }
        iterator end() { return SDAL_Iter(item_array + num_items); }

        const_iterator begin() const { return SDAL_Const_Iter(item_array); }
        const_iterator end() const { return SDAL_Const_Iter(item_array + num_items); }

        //--------------------------------------------------
        // operators
        //--------------------------------------------------
        T& operator[](size_t i) {
            if (i >= size()) {
                throw std::out_of_range(std::string("operator[]: No element at position ") + std::to_string(i));
            }
            return item_array[i];
        }

        const T& operator[](size_t i) const {
            if (i >= size()) {
                throw std::out_of_range(std::string("operator[]: No element at position ") + std::to_string(i));
            }
            return item_array[i];
        }

        //--------------------------------------------------
        // Constructors/destructor/assignment operator
        //--------------------------------------------------

        SDAL(size_t num_nodes_to_preallocate = 50): array_size(num_nodes_to_preallocate), num_items(0) {
            item_array = new T[array_size];
        }

        //--------------------------------------------------
        //copy constructor
        SDAL(const SDAL& src): SDAL(src.array_size) {
            // populate this SDAL with copies of the other SDAL's contents
            for (size_t i = 0; i != src.size(); ++i) {
                push_back(src.item_array[i]);
            }
        }

        //--------------------------------------------------
        //destructor
        ~SDAL() {
            // safely dispose of this SDAL's contents
            delete[] item_array;
        }

        //--------------------------------------------------
        //copy assignment constructor
        SDAL& operator=(const SDAL& src) {
            if (&src == this) // check for self-assignment
                return *this;     // do nothing
            delete[] item_array;
            array_size = src.array_size;
            num_items = 0;
            item_array = new T[array_size];
            // populate this SDAL with copies of the other SDAL's contents
            for (size_t i = 0; i != src.size(); ++i) {
                push_back(src.item_array[i]);
            }
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
                old_item = item_array[position];
                item_array[position] = element;
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
                embiggen_if_necessary();
                //shift remaining items right
                for (size_t i = size(); i != position; --i) {
                    item_array[i] = item_array[i - 1];
                }
                item_array[position] = element;
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
            T item;
            if (position >= size()) {
                throw std::out_of_range(std::string("remove: No element at position ") + std::to_string(position));
            } else {
                item = item_array[position];
                //shift remaining items left
                for (size_t i = position + 1; i != size(); ++i) {
                    item_array[i - 1] = item_array[i];
                }
                --num_items;
                shrink_if_necessary();
            }
            return item;
        }

        //--------------------------------------------------
        /*
            returns (without removing from the list) the element at the specified position.
        */
        T item_at(int position) const {
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
            return num_items;
        }

        //--------------------------------------------------
        /*
            removes all elements from the list.
        */
        void clear() {
            //no reason to do memory deallocation here, just overwrite the old items later and save
            //deallocation for the deconstructor
            num_items = 0;
        }

        //--------------------------------------------------
        /*
            returns true IFF one of the elements of the list matches the specified element.
        */
        bool contains(const T& element,
               bool equals(const T& a, const T& b)) const {
            bool element_in_list = false;
            for (size_t i = 0; i != size(); ++i) {
                if (equals(item_array[i], element)) {
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
    };
} // end namespace cop3530

#endif // _SDAL_H_
