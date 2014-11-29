#ifndef _PART4_H_
#define _PART4_H_

#include <cstdlib>
#include "../../common/SSLL.h"
#include "../../common/common.h"
#include "../../common/priority_queue.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type,
             typename compare_functor = hash_utils::functors::compare_functor>
    class RBST {
    private:
        compare_functor compare;
        struct Node;
        typedef Node* link;
        struct Node {
            key_type key;
            value_type value;
            size_t num_children = 0;
            size_t left_index = 0;
            size_t right_index = 0;
        };
        Node* nodes; //***note: array is 1-based so leaf nodes have child indices set to zero
        size_t free_index;
        size_t root_index;
        size_t curr_capacity;
        size_t procure_node(key_type const& key, value_type const& value) {
            //returns the index of what was the last free index
            size_t node_index = free_index;
            free_index = nodes[free_index].left_index;
            Node& n = nodes[node_index];
            n.left_index = n.right_index = 0;
            n.key = key;
            n.value = value;
            return node_index;
        }
        void insert_at_leaf(size_t& subtree_root_index, key_type const& key, value_type const& value) {
            if (subtree_root_index == 0) {
                //parent was a leaf, so create a new leaf
                subtree_root_index = procure_node(key, value);
            } else {
                //parent was not a leaf
                Node& n = nodes[subtree_root_index];
                if (rand() < RAND_MAX / (n.num_children + 1)) {
                    //randomly insert at the subtree root
                    insert_at_root(subtree_root_index, key, value);
                } else {
                    //keep going down to the base of the tree
                    if (compare(key, n.key) == -1) {
                        //key is less than subtree root's key
                        insert_at_leaf(n.left_index, key, value);
                    } else {
                        insert_at_leaf(n.right_index, key, value);
                    }
                    n.num_children++;
                }
            }
        }
        void rotate_left(size_t& subtree_root_index) {
            Node& subtree_root = nodes[subtree_root_index];
            size_t right_child_index = subtree_root.right_index;
            Node& right_child = nodes[right_child_index];
            subtree_root.right_index = right_child.left_index;
            right_child.left_index = subtree_root_index;
            subtree_root_index = right_child_index;
            //right child adopts root and its children
            right_child.num_children = subtree_root.num_children;
            //root has new children
            subtree_root.num_children = 0;
            if (subtree_root.left_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.left_index].num_children;
            if (subtree_root.right_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.right_index].num_children;
        }
        void rotate_right(size_t& subtree_root_index) {
            Node& subtree_root = nodes[subtree_root_index];
            size_t left_child_index = subtree_root.left_index;
            Node& left_child = nodes[left_child_index];
            subtree_root.left_index = left_child.right_index;
            left_child.right_index = subtree_root_index;
            subtree_root_index = left_child_index;
            //right child adopts root and its children
            left_child.num_children = subtree_root.num_children;
            //root has new children
            subtree_root.num_children = 0;
            if (subtree_root.left_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.left_index].num_children;
            if (subtree_root.right_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.right_index].num_children;
        }
        void insert_at_root(size_t& subtree_root_index, key_type const& key, value_type const& value) {
            if (subtree_root_index == 0) {
                //parent was a leaf, so create a new leaf
                subtree_root_index = procure_node(key, value);
            } else {
                //parent was not a leaf
                Node& n = nodes[subtree_root_index];
                //keep going down to the base of the tree
                if (compare(key, n.key) == -1) {
                    //key is less than subtree root's key
                    insert_at_root(n.left_index, key, value);
                    //new node currently a new child of subtree root, so increment
                    //the subtree root's number of children before rotating - new node
                    //will adopt the root and its children and will take on the value
                    //of its num_children
                    n.num_children++;
                    rotate_right(subtree_root_index);
                } else {
                    insert_at_root(n.right_index, key, value);
                    n.num_children++;
                    rotate_right(subtree_root_index);
                }
            }
        }
    public:
        /*
            The constructor will allocate an array of capacity (binary
            tree) nodes. Then make a chain from all the nodes (e.g.,
            make node 1 the left child of node 0, make node 2 the left
            child of node 1, &c.&this is the initial free list. You will
            also need two variables: one containing the index of the
            tree's root node [initially -1], the other containing the
            index of the free node list's head [initially 0, if you chained
            nodes in the order suggested above].
        */
        RBST(size_t capacity):
            curr_capacity(curr_capacity)
        {
            if (capacity == 0) {
                throw std::domain_error("capacity must be at least 1");
            }
            nodes = new Node[capacity + 1];
            //Since I use size_t to hold the node indices, I make the node array
            //1-based, with child index of 0 indicating that the current node is a leaf
            for (size_t i = 1; i != capacity; ++i)
                nodes[i].left_index = i + 1;
            free_index = 1;
            root_index = 0;
        }
        /*
            if there is space available, adds the specified key/value-pair to the tree
            and returns the number of nodes visited, V; otherwise returns -1 * V. If an
            item already exists in the tree with the same key, replace its value.
        */
        bool insert(key_type const& key, value_type const& value) {
            insert_at_leaf(root_index, key, value);
            return true;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the tree, stores
            it's value in value, and returns the number of probes required, V; otherwise returns -1 * V.
        */
        bool remove(key_type const& key, value_type& value) {
            return true;
        }
        /*
            if there is an item matching key, stores it's value in value, and returns the number
            of nodes visited, V; otherwise returns -1 * V. Regardless, the item remains in the tree.
        */
        bool search(key_type const& key, value_type& value) {
            return true;
        }
        /*
            removes all items from the map.
        */
        void clear() {
        }
        /*
            returns true IFF the map contains no elements.
        */
        bool is_empty() const {
            return size() == 0;
        }
        /*
            returns the number of slots in the backing array.
        */
        size_t capacity() const {
            return curr_capacity;
        }
        /*
            returns the number of items actually stored in the tree.
        */
        size_t size() const {
            Node const& n = nodes[root_index];
            return 1 + n.num_children;
        }
        /*
            [not a regular RBST operation, but specific to this implementation]
            returns the tree's load factor: load = size / capacity.
        */
        double load() const {
            return static_cast<double>(size()) / capacity();
        }
        /*
            prints the tree in the following format:
            +--[tiger]
            |  |
            |  |  +--[panther]
            |  |  |
            |  +--[ocelot]
            |     |
            |     +--[lion]
            |
            [leopard]
            |
            |     +--[house cat]
            |     |
            |  +--[cougar]
            |  |
            +--[cheetah]
               |
               +--[bobcat]
        */
        std::ostream& print(std::ostream& out) const {
            /*size_t cap = capacity();
            out << '[';
            for (size_t i = 0; i != cap; ++i) {
                if (slots[i].is_occupied) {
                    out << slots[i].item.key.raw();
                } else {
                    out << "-";
                }
                if (i + 1 < cap)
                    out << '|';
            }
            out << ']';
            return out;*/
        }

        priority_queue<hash_utils::ClusterInventory> cluster_distribution() {
            SSLL<hash_utils::ClusterInventory> clusters;

        }
    };
}

#endif
