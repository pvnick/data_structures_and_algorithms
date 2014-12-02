#ifndef _RBST_H_
#define _RBST_H_


#include <cstdlib>
#include <sstream>
#include "../../common/CDAL.h"
#include "../../common/common.h"
#include "../../common/priority_queue.h"
#include "bst.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type,
             typename compare_functor = hash_utils::functors::compare_functor>
    class RBST: public BST<key_type, value_type, compare_functor> {
    /*
        Within the RBST insert_at_leaf method, the recursive execution path is randomly redirected
        to insert at the root. Therefore, we simply inherit from a generic BST class and wrap the
        insert_at_leaf method with that potential alternative execution path
    */
    private:
        using super = BST<key_type, value_type, compare_functor>;
        using typename super::Node;
        int insert_at_leaf(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                           size_t& subtree_root_index,
                           key_type const& key,
                           value_type const& value,
                           bool& found_key)
        {
            //parent was not a leaf
            Node& subtree_root = this->nodes[subtree_root_index];
            if (rand() < RAND_MAX / (subtree_root.num_children + 1)) {
                //randomly insert at the subtree root
                nodes_visited = insert_at_root(nodes_visited, subtree_root_index, key, value, found_key);
            } else {
                nodes_visited = super::insert_at_leaf(nodes_visited, subtree_root_index, key, value, found_key);
            }
            return nodes_visited;
        }
        int insert_at_root(size_t nodes_visited,
                           size_t& subtree_root_index,
                           key_type const& key,
                           value_type const& value,
                           bool& found_key)
        {
            if (subtree_root_index == 0) {
                //parent was a leaf, so create a new leaf
                subtree_root_index = this->procure_node(key, value);
            } else {
                //parent was not a leaf
                Node& subtree_root = this->nodes[subtree_root_index];
                ++nodes_visited;
                //keep going down to the base of the tree
                switch (this->compare(key, subtree_root.key)) {
                case -1:
                    //key is less than subtree root's key
                    nodes_visited = insert_at_root(nodes_visited, subtree_root.left_index, key, value, found_key);
                    if ( ! found_key) {
                        //new node currently a new child of subtree root, so increment
                        //the subtree root's number of children before rotating - new node
                        //will adopt the root and its children and will take on the value
                        //of its num_children
                        subtree_root.num_children++;

                        //current subtree root may have had its height changed, so update that before
                        //promoting the new node
                        subtree_root.update_height(this->nodes);
                        this->rotate_right(subtree_root_index);
                    }
                    break;
                case 1:
                    //key is greater than subtree root's key
                    nodes_visited = insert_at_root(nodes_visited, subtree_root.right_index, key, value, found_key);
                    if ( ! found_key) {
                        subtree_root.num_children++;

                        //current subtree root may have had its height changed, so update that before
                        //promoting the new node
                        subtree_root.update_height(this->nodes);
                        this->rotate_left(subtree_root_index);
                    }
                    break;
                case 0:
                    //found key, replace the value
                    subtree_root.value = value;
                    found_key = true;
                    break;
                default:
                    throw std::domain_error("insert_at_root: Unexpected compare() function return value");
                }
            }
            return nodes_visited;
        }
    public:
        RBST(size_t capacity): super(capacity) {}
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
            size_t nodes_visited = insert_at_leaf(0, this->root_index, key, value, found_key);
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            return nodes_visited;
        }
    };
}

#endif
