#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include "SSLL.h"
#include "common.h"

namespace cop3530 {
    //this class takes a simple singly linked list containing clusters and exposes
    //a method (get_next_item) which returns the clusters is order of ascending size
    template<typename T,
             typename PriorityCompare = cop3530::hash_utils::functors::cluster_size_less_predicate<T>>
    class priority_queue {
    private:
        PriorityCompare first_arg_higher_priority;
        T* tree;
        size_t num_items = 0;
        void add_to_queue(T const& item) {
            tree[++num_items] = item;
            fix_up(num_items);
        }
        void fix_up(size_t index) {
            while (index > 1
                   && first_arg_higher_priority(tree[index], tree[index / 2]))
            {
                std::swap(tree[index / 2], tree[index]);
                index /= 2;
            }
        }
        void fix_down() {
            size_t parent_index = 1;
            while (2 * parent_index <= num_items) {
                size_t left_index = 2 * parent_index;
                size_t right_index = left_index + 1;
                size_t higher_priority_index = left_index;
                if (right_index <= num_items
                    && first_arg_higher_priority(tree[right_index], tree[left_index]))
                {
                    higher_priority_index = right_index;
                }
                if ( ! first_arg_higher_priority(tree[higher_priority_index], tree[parent_index]))
                    //no more items to elevate
                    break;
                std::swap(tree[parent_index], tree[higher_priority_index]);
                parent_index = higher_priority_index;
            }
        }
    public:
        //take a linked list of cluster descriptors and add each to the priority queue
        priority_queue(SSLL<T> const& items) {
            tree = new T[items.size() + 1]; //tree should be 1-based for easy math
            typename SSLL<T>::const_iterator iter = items.begin();
            typename SSLL<T>::const_iterator iter_end = items.end();
            for (; iter != iter_end; ++iter)
                add_to_queue(*iter);
        }
        ~priority_queue() {
            delete[] tree;
        }
        priority_queue(priority_queue const& src) {
            size_t other_pq_size = src.size();
            tree = new T[other_pq_size + 1];
            for (int i = 1; i <= other_pq_size; ++i)
                tree[i] = src.tree[i];
        }
        T get_next_item() {
            std::swap(tree[1], tree[num_items]);
            fix_down();
            return tree[num_items--];
        }
        size_t size() {
            return num_items;
        }
        bool empty() {
            return num_items == 0;
        }
    };
}

#endif // _PRIORITY_QUEUE_H_
