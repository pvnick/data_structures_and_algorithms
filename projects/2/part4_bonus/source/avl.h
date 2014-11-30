#ifndef _AVL_H_
#define _AVL_H_

#include <cstdlib>
#include <sstream>
#include "../../common/SSLL.h"
#include "../../common/CDAL.h"
#include "../../common/common.h"
#include "../../common/priority_queue.h"
#include "../../part4/source/rbst.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type,
             typename compare_functor = hash_utils::functors::compare_functor>
    class AVL: public RBST<key_type, value_type, compare_functor> {
        using typename RBST<key_type, value_type, compare_functor>::Node;
        int insert_balanced(size_t& subtree_root_index, key_type const& key, value_type const& value, bool& found_key) {
            if (subtree_root_index == 0) {
                //parent was a leaf, so create a new leaf
                subtree_root_index = this->procure_node(key, value);
            } else {
                //parent was not a leaf
                //keep going down to the base of the tree
                Node& n = this->nodes[subtree_root_index];
                switch (this->compare(key, n.key)) {
                case -1:
                    //key is less than subtree root's key
                    this->insert_balanced(n.left_index, key, value, found_key);
                    if ( ! found_key) {
                        //given key is unique to the tree, so a new node was added
                        n.num_children++;
                        n.update_height(this->nodes);
                        balance(subtree_root_index);
                    }
                    break;
                case 1:
                    //key is greater than subtree root's key
                    insert_balanced(n.right_index, key, value, found_key);
                    if ( ! found_key) {
                        //given key is unique to the tree, so a new node was added
                        n.num_children++;
                        n.update_height(this->nodes);
                        balance(subtree_root_index);
                    }
                    break;
                case 0:
                    //found key, replace the value
                    n.value = value;
                    found_key = true;
                    break;
                default:
                    throw std::runtime_error("Unexpected compare() function return value");
                }
            }
        }
        void balance(size_t& subtree_root_index) {
            Node& root = this->nodes[subtree_root_index];
            int root_bal_fact = root.balance_factor(this->nodes);
            if (root_bal_fact == -2) {
                //right subtree is too heavy
                size_t& right_index = root.right_index;
                Node& right_child = this->nodes[right_index];
                switch(right_child.balance_factor(this->nodes)) {
                case 1:
                    //right left
                    this->rotate_right(right_index);
                    this->rotate_left(subtree_root_index);
                    break;
                case -1: //todo: see if this needs to be 0 also
                    //right right
                    this->rotate_left(subtree_root_index);
                    break;
                default:
                    throw std::runtime_error(std::string("Unexpected balance factor with heavy right subtree: ")
                                             + std::to_string(right_child.balance_factor(this->nodes)));
                }
            } else if (root_bal_fact == 2) {
                //left subtree is too heavy
                size_t& left_index = root.left_index;
                Node& left_child = this->nodes[left_index];
                switch(left_child.balance_factor(this->nodes)) {
                case -1:
                    //left right
                    this->rotate_left(left_index);
                    this->rotate_right(subtree_root_index);
                    break;
                case 1: //todo: see if this needs to be 0 also
                    //left left
                    this->rotate_right(subtree_root_index);
                    break;
                default:
                    throw std::runtime_error(std::string("Unexpected balance factor with heavy left subtree: ")
                                             + std::to_string(left_child.balance_factor(this->nodes)));
                }
            } else if (std::abs(root_bal_fact > 2)) {
                throw std::runtime_error(std::string("Unexpected balance factor when checking for heavy subtree: ")
                                         + std::to_string(root_bal_fact));
            }
        }
    public:
        AVL(size_t capacity): RBST<key_type, value_type, compare_functor>(capacity) {}
        /*
            if there is space available, adds the specified key/value-pair to the tree
            and returns the number of nodes visited, V; otherwise returns -1 * V. If an
            item already exists in the tree with the same key, replace its value.
        */
        bool insert(key_type const& key, value_type const& value) {
            if (this->size() == this->capacity())
                //no more space
                return 0;
            bool found_key = false;
            insert_balanced(this->root_index, key, value, found_key);
            return true;
        }
    };
}

#endif
