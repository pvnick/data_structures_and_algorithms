#ifndef _PART4_H_
#define _PART4_H_

#include <cstdlib>
#include <sstream>
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
            size_t num_children;
            size_t left_index;
            size_t right_index;
            void reset(key_type const new_key, value_type const& new_value) {
                left_index = right_index = 0;
                num_children = 0;
                key = new_key;
                value = new_value;
            }
        };
        Node* nodes; //***note: array is 1-based so leaf nodes have child indices set to zero
        size_t free_index;
        size_t root_index;
        size_t curr_capacity;
        size_t remove_smallest_key_node_index(size_t& subtree_root_index) {
            //returns the index of the node with the smallest key, while
            //setting its parent's left child index to zero
            Node& subtree_root = nodes[subtree_root_index];
            size_t smallest_key_node_index = 0;
            if (subtree_root_index == 0) {
                throw std::runtime_error("Expected to find a valid node, but didn't");
            } else {
                if (subtree_root.left_index) {
                    smallest_key_node_index = remove_smallest_key_node_index(subtree_root.left_index);
                    subtree_root.num_children--;
                } else {
                    smallest_key_node_index = subtree_root_index;
                    subtree_root_index = subtree_root.right_index;
                }
            }
            return smallest_key_node_index;
        }
        size_t remove_largest_key_node_index(size_t& subtree_root_index) {
            //returns the index of the node with the largest key, while
            //setting its parent's right child index to zero
            Node& subtree_root = nodes[subtree_root_index];
            size_t largest_key_node_index = 0;
            if (subtree_root_index == 0) {
                throw std::runtime_error("Expected to find a valid node, but didn't");
            } else {
                if (subtree_root.right_index) {
                    largest_key_node_index = remove_largest_key_node_index(subtree_root.right_index);
                    subtree_root.num_children--;
                } else {
                    largest_key_node_index = subtree_root_index;
                    subtree_root_index = subtree_root.left_index;
                }
            }
            return largest_key_node_index;
        }
        int do_remove(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                      size_t& subtree_root_index,
                      key_type const& key,
                      value_type& value)
        {
            if (subtree_root_index == 0)
                //key not found
                return -1 * nodes_visited;
            else {
                Node& subtree_root = nodes[subtree_root_index];
                //keep going down to the base of the tree
                int cmp = compare(key, subtree_root.key);
                int new_nodes_visited;
                size_t index_to_delete;
                switch (cmp) {
                case -1:
                    //key is less than subtree root's key
                    new_nodes_visited = do_remove(nodes_visited + 1, subtree_root.left_index, key, value);
                    subtree_root.num_children--;
                    return new_nodes_visited;
                case 1:
                    //key is greater than subtree root's key
                    new_nodes_visited = do_remove(nodes_visited + 1, subtree_root.right_index, key, value);
                    subtree_root.num_children--;
                    return new_nodes_visited;
                case 0:
                    //found key, remove the node
                    value = subtree_root.value;
                    index_to_delete = subtree_root_index;
                    if (subtree_root.right_index || subtree_root.left_index) {
                        //subtree has at least one child
                        if (subtree_root.right_index)
                            //replace the root with the smallest-keyed node in the right subtree
                            subtree_root_index = remove_smallest_key_node_index(subtree_root.right_index);
                        else if (subtree_root.left_index)
                            //replace the root with the largest-keyed node in the left subtree
                            subtree_root_index = remove_largest_key_node_index(subtree_root.left_index);
                        //have the new root adopt the old root's children
                        Node& new_root = nodes[subtree_root_index];
                        new_root.left_index = subtree_root.left_index;
                        new_root.right_index = subtree_root.right_index;
                        //the new root has the same number of children as the old root, minus one
                        new_root.num_children = subtree_root.num_children - 1;
                    } else
                        //neither subtree doesnt exists, so just delete the node
                        subtree_root_index = 0;
                    add_node_to_free_tree(index_to_delete);
                    return nodes_visited + 1;
                default:
                    throw std::runtime_error("Unexpected compare() function return value");
                }
            }
        }
        void write_subtree(size_t subtree_root_index,
                           std::string lines[],
                           size_t root_line_index,
                           size_t lbound_line_index /*inclusive*/,
                           size_t ubound_line_index /*exclusive*/) const
        {
            Node subtree_root = nodes[subtree_root_index];
            std::ostringstream oss;
            //print the node
            //todo: fix this to only print the key
            oss << "[" << subtree_root.key << ", " << subtree_root.value << ", " << subtree_root.num_children << "]";
            lines[root_line_index] += oss.str();
            //print the right descendents
            if (subtree_root.right_index > 0) {
                //at least 1 right child
                size_t top_dashes = 1;
                Node const& right_child = nodes[subtree_root.right_index];
                if (right_child.left_index > 0) {
                    //right child has at least 1 left child
                    Node const& right_left_child = nodes[right_child.left_index];
                    top_dashes += 2 * (1 + right_left_child.num_children);
                }
                size_t top_line_index = root_line_index - 1;
                while (top_line_index >= root_line_index - top_dashes)
                    lines[top_line_index--] += "|  ";
                size_t right_child_line_index = top_line_index;
                lines[top_line_index--] += "+--";
                while (top_line_index >= lbound_line_index)
                    lines[top_line_index--] += "   ";
                write_subtree(subtree_root.right_index,
                              lines,
                              right_child_line_index,
                              lbound_line_index,
                              root_line_index);
            }
            //print the left descendents
            if (subtree_root.left_index > 0) {
                //at least 1 left child
                size_t bottom_dashes = 1;
                Node const& left_child = nodes[subtree_root.left_index];
                if (left_child.right_index > 0) {
                    //left child has at least 1 right child
                    Node const& left_right_child = nodes[left_child.right_index];
                    bottom_dashes += 2 * (1 + left_right_child.num_children);
                }
                size_t bottom_line_index = root_line_index + 1;
                while (bottom_line_index <= root_line_index + bottom_dashes)
                    lines[bottom_line_index++] += "|  ";
                size_t left_child_line_index = bottom_line_index;
                lines[bottom_line_index++] += "+--";
                while (bottom_line_index < ubound_line_index)
                    lines[bottom_line_index++] += "   ";
                write_subtree(subtree_root.left_index,
                              lines,
                              left_child_line_index,
                              root_line_index + 1,
                              ubound_line_index);
            }
        }
        void add_node_to_free_tree(size_t node_index) {
            Node& to_free = nodes[node_index];
            to_free.right_index = 0;
            to_free.left_index = free_index;
            free_index = node_index;
        }
        size_t procure_node(key_type const& key, value_type const& value) {
            //returns the index of what was the last free index
            size_t node_index = free_index;
            free_index = nodes[free_index].left_index;
            Node& n = nodes[node_index];
            n.reset(key, value);
            return node_index;
        }
        int insert_at_leaf(size_t& subtree_root_index, key_type const& key, value_type const& value, bool& found_key) {
            if (subtree_root_index == 0) {
                //parent was a leaf, so create a new leaf
                subtree_root_index = procure_node(key, value);
            } else {
                //parent was not a leaf
                Node& n = nodes[subtree_root_index];
                if (rand() < RAND_MAX / (n.num_children + 1)) {
                    //randomly insert at the subtree root
                    insert_at_root(subtree_root_index, key, value, found_key);
                } else {
                    //keep going down to the base of the tree
                    int cmp = compare(key, n.key);
                    switch (cmp) {
                    case -1:
                        //key is less than subtree root's key
                        insert_at_leaf(n.left_index, key, value, found_key);
                        if ( ! found_key)
                            n.num_children++;
                        break;
                    case 1:
                        //key is greater than subtree root's key
                        insert_at_leaf(n.right_index, key, value, found_key);
                        if ( ! found_key)
                            n.num_children++;
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
        int insert_at_root(size_t& subtree_root_index, key_type const& key, value_type const& value, bool& found_key) {
            if (subtree_root_index == 0) {
                //parent was a leaf, so create a new leaf
                subtree_root_index = procure_node(key, value);
            } else {
                //parent was not a leaf
                Node& n = nodes[subtree_root_index];
                //keep going down to the base of the tree
                int cmp = compare(key, n.key);
                switch (cmp) {
                case -1:
                    //key is less than subtree root's key
                    insert_at_root(n.left_index, key, value, found_key);
                    if ( ! found_key) {
                        //new node currently a new child of subtree root, so increment
                        //the subtree root's number of children before rotating - new node
                        //will adopt the root and its children and will take on the value
                        //of its num_children
                        n.num_children++;
                        rotate_right(subtree_root_index);
                    }
                    break;
                case 1:
                    //key is greater than subtree root's key
                    insert_at_root(n.right_index, key, value, found_key);
                    if ( ! found_key) {
                        n.num_children++;
                        rotate_left(subtree_root_index);
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
        int do_search(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                      size_t subtree_root_index,
                      key_type const& key,
                      value_type value) const
        {
            if (subtree_root_index == 0)
                //key not found
                return -1 * nodes_visited;
            else {
                Node const& subtree_root = nodes[subtree_root_index];
                int cmp = compare(key, subtree_root.key);
                switch (cmp) {
                case -1:
                    //key is less than subtree root key
                    return do_search(nodes_visited + 1, subtree_root.left_index, key, value);
                case 1:
                    //key is greater than subtree root key
                    return do_search(nodes_visited + 1, subtree_root.right_index, key, value);
                case 0:
                    //found key
                    value = subtree_root.value;
                    return nodes_visited + 1;
                    break;
                default:
                    throw std::runtime_error("Unexpected compare() function return value");
                }
            }
        }
    public:
        /*
            The constructor will allocate an array of capacity (binary
            tree) nodes. Then make a chain from all the nodes (e.g.,
            make node 2 the left child of node 1, make node 3 the left
            child of node 2, &c. this is the initial free list.
        */
        RBST(size_t capacity):
            curr_capacity(capacity)
        {
            if (capacity == 0) {
                throw std::domain_error("capacity must be at least 1");
            }
            nodes = new Node[capacity + 1];
            clear();
        }
        /*
            if there is space available, adds the specified key/value-pair to the tree
            and returns the number of nodes visited, V; otherwise returns -1 * V. If an
            item already exists in the tree with the same key, replace its value.
        */
        bool insert(key_type const& key, value_type const& value) {
            if (size() == capacity())
                //no more space
                return 0;
            bool found_key = false;
            insert_at_leaf(root_index, key, value, found_key);
            return true;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the tree, stores
            it's value in value, and returns the number of probes required, V; otherwise returns -1 * V.
        */
        int remove(key_type const& key, value_type& value) {
            return do_remove(0, root_index, key, value);
        }
        /*
            if there is an item matching key, stores it's value in value, and returns the number
            of nodes visited, V; otherwise returns -1 * V. Regardless, the item remains in the tree.
        */
        int search(key_type const& key, value_type& value) {
            return do_search(0, root_index, key, value);
        }
        /*
            removes all items from the map.
        */
        void clear() {
            //Since I use size_t to hold the node indices, I make the node array
            //1-based, with child index of 0 indicating that the current node is a leaf
            for (size_t i = 1; i != capacity(); ++i)
                nodes[i].left_index = i + 1;
            free_index = 1;
            root_index = 0;
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
            if (root_index == 0)
                return out;
            size_t num_lines = size() * 2 - 1;
            //lines is 1-based because integer overflows are pernicious and sneaky
            std::string lines[num_lines + 1];
            Node const& root = nodes[root_index];
            size_t root_line_index = 1;
            if (root.right_index) {
                root_line_index += 2 * (1 + nodes[root.right_index].num_children);
            }
            write_subtree(root_index, lines, root_line_index, 1, num_lines + 1);
            for (size_t i = 1; i <= num_lines; ++i)
                out << lines[i] << std::endl;
            return out;
        }

        priority_queue<hash_utils::ClusterInventory> cluster_distribution() {
            SSLL<hash_utils::ClusterInventory> clusters;

        }
    };
}

#endif
