#ifndef _AVL_H_
#define _AVL_H_

#include <cstdlib>
#include <sstream>
#include "../../common/CDAL.h"
#include "../../common/common.h"
#include "../../common/priority_queue.h"
#include "../../part4/source/bst.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type>
    class AVL: public BST<key_type, value_type> {
    /*
        The trick to AVL is to perform standard BST operations, but wrap recursive methods that might unbalance
        the tree with methods that rebalance the tree after performing those operations. Thus the balance factor
        of any given node stays within [-1, 1]. To that end we simply inherit from a BST base class that tracks
        changes in subtree height and overwrite the needed virtual methods.
    */
    private:
        using super = BST<key_type, value_type>;
        using typename super::Node;
        typedef hash_utils::Key<key_type> Key;
        typedef hash_utils::Value<value_type> Value;
        int insert_at_leaf(size_t nodes_visited,
                           size_t& subtree_root_index,
                           Key const& key,
                           Value const& value,
                           bool& found_key)
        {
            nodes_visited = super::insert_at_leaf(nodes_visited, subtree_root_index, key, value, found_key);
            balance(subtree_root_index);
            return nodes_visited;
        }
        size_t remove_smallest_key_node_index(size_t& subtree_root_index) {
            size_t smallest_key_node_index = super::remove_smallest_key_node_index(subtree_root_index);
            balance(subtree_root_index);
            return smallest_key_node_index;
        }
        size_t remove_largest_key_node_index(size_t& subtree_root_index) {
            size_t largest_key_node_index = super::remove_largest_key_node_index(subtree_root_index);
            balance(subtree_root_index);
            return largest_key_node_index;
        }
        int do_remove(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                      size_t& subtree_root_index,
                      Key const& key,
                      Value& value,
                      bool& found_key)
        {
            nodes_visited = super::do_remove(nodes_visited, subtree_root_index, key, value, found_key);
            balance(subtree_root_index);
            return nodes_visited;
        }
        void balance(size_t& subtree_root_index) {
            if (subtree_root_index == 0) return;
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
                case -1:
                case 0:
                    //right right
                    this->rotate_left(subtree_root_index);
                    break;
                default:
                    throw std::domain_error(std::string("Unexpected balance factor with heavy right subtree: ")
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
                case 1:
                case 0:
                    //left left
                    this->rotate_right(subtree_root_index);
                    break;
                default:
                    throw std::domain_error(std::string("Unexpected balance factor with heavy left subtree: ")
                                             + std::to_string(left_child.balance_factor(this->nodes)));
                }
            } else if (std::abs(root_bal_fact > 2)) {
                throw std::domain_error(std::string("Unexpected balance factor when checking for heavy subtree: ")
                                         + std::to_string(root_bal_fact));
            }
        }
        void do_validate_avl_balance(size_t subtree_root_index) const {
            if (_DEBUG_) {
                if (subtree_root_index == 0) return;
                Node const& n = this->nodes[subtree_root_index];
                if (abs(n.balance_factor(this->nodes)) > 1)
                    throw std::domain_error("Unexpected unbalanced tree while checking balance factor of all tree nodes");
                do_validate_avl_balance(n.left_index);
                do_validate_avl_balance(n.right_index);
            }
        }
        void validate_avl_balance() {
            if (_DEBUG_)
                do_validate_avl_balance(this->root_index);
        }
    public:
        AVL(size_t capacity): super(capacity) {}
        /*
            if there is space available, adds the specified key/value-pair to the tree
            and returns the number of nodes visited, V; otherwise returns -1 * V. If an
            item already exists in the tree with the same key, replace its value.
        */
        int insert(key_type const& key, value_type const& value) {
            if (this->size() == this->capacity())
                //no more space
                return -1 * this->size();
            bool found_key = false;
            Key k(key);
            Value v(value);
            int nodes_visited = insert_at_leaf(0, this->root_index, k, v, found_key);
            validate_avl_balance();
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            return nodes_visited;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the tree, stores
            it's value in value, and returns the number of probes required, V; otherwise returns -1 * V.
        */
        int remove(key_type const& key, value_type& value) {
            if (this->is_empty())
                return 0;
            bool found_key = false;
            Key k(key);
            Value v(value);
            int nodes_visited = do_remove(0, this->root_index, k, v, found_key);
            validate_avl_balance();
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            if (found_key)
                value = v.raw();
            return found_key ? nodes_visited : -1 * nodes_visited;
        }
    };
}

#endif
