#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include "SDAL.h"
#include "common.h"

namespace cop3530 {
    //this class takes a simple singly linked list containing clusters and exposes
    //a method (get_next_item) which returns the clusters is order of ascending size
    template<typename T,
             typename PriorityCompare = cop3530::hash_utils::ClusterInventory::cluster_size_less_predicate>
    class priority_queue {
    private:
        PriorityCompare first_arg_higher_priority;
        //SDAL has all the benefits of std::vector while having the added benefit of being legal to use in cop3530
        SDAL<T> tree;
        size_t num_items = 0;
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
        priority_queue(size_t preallocation_size = 100): tree(preallocation_size + 1) {
            T empty_item;
            tree.push_back(empty_item);
        }
        priority_queue(priority_queue const& src) {
            tree = src.tree;
            num_items = src.num_items;
        }
        T get_next_item() {
            std::swap(tree[1], tree[num_items]);
            T ret = tree[num_items--];
            fix_down();
            return ret;
        }
        void add_to_queue(T const& item) {
            tree.push_back(item);
            num_items++;
            fix_up(num_items);
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
