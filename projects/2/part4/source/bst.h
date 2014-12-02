#ifndef _BST_H_
#define _BST_H_

#include <cstdlib>
#include <sstream>
#include "../../common/CDAL.h"
#include "../../common/common.h"
#include "../../common/priority_queue.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type,
             typename compare_functor = hash_utils::functors::compare_functor>
    class BST {
    protected: //let RBST and AVL inherit everything
        compare_functor compare;
        struct Node;
        typedef Node* link;
        struct Node {
            key_type key;
            value_type value;
            size_t num_children;
            size_t left_index;
            size_t right_index;
            size_t height; //height tracking coded in this class, but not used (for AVL, which is this class with self-balancing)
            bool is_occupied;
            size_t get_height_recursive(Node* nodes) {
                //this function is for debugging purposes, does recursive traversal to find the correct height
                //todo: delete this function
                size_t left_height = 0, right_height = 0;
                size_t calculated_height = 0;
                if (left_index)
                    left_height = nodes[left_index].get_height_recursive(nodes);
                if (right_index)
                    right_height = nodes[right_index].get_height_recursive(nodes);
                calculated_height = 1 + std::max(left_height, right_height);
                return calculated_height;
            }
            void update_height(Node* nodes) {
                //note: this method depends on the left and right subtree heights being correct
                size_t left_height = 0, right_height = 0;
                if (left_index)
                    left_height = nodes[left_index].height;
                if (right_index)
                    right_height = nodes[right_index].height;
                height = 1 + std::max(left_height, right_height);
                //todo: delete the following expensive check, or move it into DEBUG condition
                size_t calculated_height = get_height_recursive(nodes);
                if (calculated_height != height) {
                    std::ostringstream msg;
                    msg << "Manually calculated height, " << calculated_height << ", different than tracked height, " << height;
                    throw std::runtime_error(msg.str());
                }
            }
            void disable_and_adopt_free_tree(size_t free_index) {
                is_occupied = false;
                height = 0;
                num_children = 0;
                right_index = 0;
                left_index = free_index;
            }
            void reset_and_enable(key_type const new_key, value_type const& new_value) {
                is_occupied = true;
                height = 1; //self
                left_index = right_index = 0;
                num_children = 0;
                key = new_key;
                value = new_value;
            }
            int balance_factor(const Node* nodes) const {
                size_t left_height = 0, right_height = 0;
                if (left_index)
                    left_height = nodes[left_index].height;
                if (right_index)
                    right_height = nodes[right_index].height;
                return static_cast<long int>(left_height) - static_cast<long int>(right_height);
            }
        };
        Node* nodes; //***note: array is 1-based so leaf nodes have child indices set to zero
        size_t free_index;
        size_t root_index;
        size_t curr_capacity;
        virtual size_t remove_smallest_key_node_index(size_t& subtree_root_index) {
            //returns the index of the node with the smallest key, while
            //setting its parent's left child index to the smallest key node's
            //right child index. recursion downward through this function updates
            //the heights of the nodes it traverses
            Node& subtree_root = nodes[subtree_root_index];
            size_t smallest_key_node_index = 0;
            if (subtree_root_index == 0) {
                throw std::logic_error("Expected to find a valid node, but didn't");
            } else {
                if (subtree_root.left_index) {
                    smallest_key_node_index = remove_smallest_key_node_index(subtree_root.left_index);
                    subtree_root.num_children--;
                    subtree_root.update_height(nodes);
                } else {
                    smallest_key_node_index = subtree_root_index;
                    subtree_root_index = subtree_root.right_index;
                }
            }
            return smallest_key_node_index;
        }
        virtual size_t remove_largest_key_node_index(size_t& subtree_root_index) {
            //returns the index of the node with the largest key, while
            //setting its parent's right child index to the largest key node's
            //left child index. recursion downward through this function updates
            //the heights of the nodes it traverses
            Node& subtree_root = nodes[subtree_root_index];
            size_t largest_key_node_index = 0;
            if (subtree_root_index == 0) {
                throw std::logic_error("Expected to find a valid node, but didn't");
            } else {
                if (subtree_root.right_index) {
                    largest_key_node_index = remove_largest_key_node_index(subtree_root.right_index);
                    subtree_root.num_children--;
                    subtree_root.update_height(nodes);
                } else {
                    largest_key_node_index = subtree_root_index;
                    subtree_root_index = subtree_root.left_index;
                }
            }
            return largest_key_node_index;
        }
        virtual void remove_node(size_t& subtree_root_index) {
            Node& subtree_root = nodes[subtree_root_index];
            size_t index_to_delete = subtree_root_index;
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
                //removing the smallest/largest-keyed node from the old root has the effect of
                //updating the heights of the old root's relevant subtrees (which the new root
                //just adopted), so we can update the new root's height now
                new_root.update_height(nodes);
            } else
                //neither subtree exists, so just delete the node
                subtree_root_index = 0;
            //node has been disowned by all ancestors, and has disowned all descendents, so free it
            add_node_to_free_tree(index_to_delete);
        }
        virtual int do_remove(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                              size_t& subtree_root_index,
                              key_type const& key,
                              value_type& value,
                              bool& found_key)
        {
            if (subtree_root_index == 0)
                //key not found
                nodes_visited *= -1;
            else {
                Node& subtree_root = nodes[subtree_root_index];
                ++nodes_visited;
                //keep going down to the base of the tree
                switch (compare(key, subtree_root.key)) {
                case -1:
                    //key is less than subtree root's key
                    nodes_visited = do_remove(nodes_visited, subtree_root.left_index, key, value, found_key);
                    if (found_key) {
                        //found the desired node and delete it
                        subtree_root.num_children--;
                        //left child changed, so recompute subtree height
                        subtree_root.update_height(nodes);
                    }
                    break;
                case 1:
                    //key is greater than subtree root's key
                    nodes_visited = do_remove(nodes_visited, subtree_root.right_index, key, value, found_key);
                    if (found_key) {
                        //found the desired node and delete it
                        subtree_root.num_children--;
                        //right child changed, so recompute subtree height
                        subtree_root.update_height(nodes);
                    }
                    break;
                case 0:
                    //found key, remove the node
                    found_key = true;
                    value = subtree_root.value;
                    remove_node(subtree_root_index);
                    break;
                default:
                    throw std::domain_error("Unexpected compare() function return value");
                }
            }
            return nodes_visited;
        }
        void write_subtree_buffer(size_t subtree_root_index,
                                  CDAL<std::string>& buffer_lines,
                                  size_t root_line_index,
                                  size_t lbound_line_index /*inclusive*/,
                                  size_t ubound_line_index /*exclusive*/) const
        {
            Node subtree_root = nodes[subtree_root_index];
            std::ostringstream oss;
            //print the node
            //todo: fix this to only print the key
            oss << "[" << subtree_root.key << ": val=" << subtree_root.value << ", children=" << subtree_root.num_children << ", height=" << subtree_root.height << ", bal fact=" << subtree_root.balance_factor(nodes) << "]";
            //oss << "[" << subtree_root.key << ", " << subtree_root.height << "]";
            buffer_lines[root_line_index] += oss.str();
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
                    buffer_lines[top_line_index--] += "|  ";
                size_t right_child_line_index = top_line_index;
                buffer_lines[top_line_index--] += "+--";
                while (top_line_index >= lbound_line_index)
                    buffer_lines[top_line_index--] += "   ";
                write_subtree_buffer(subtree_root.right_index,
                                     buffer_lines,
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
                    buffer_lines[bottom_line_index++] += "|  ";
                size_t left_child_line_index = bottom_line_index;
                buffer_lines[bottom_line_index++] += "+--";
                while (bottom_line_index < ubound_line_index)
                    buffer_lines[bottom_line_index++] += "   ";
                write_subtree_buffer(subtree_root.left_index,
                                     buffer_lines,
                                     left_child_line_index,
                                     root_line_index + 1,
                                     ubound_line_index);
            }
        }
        void add_node_to_free_tree(size_t node_index) {
            nodes[node_index].disable_and_adopt_free_tree(free_index);
            free_index = node_index;
        }
        size_t procure_node(key_type const& key, value_type const& value) {
            //updates the free index to the first free node's left child (while transforming that first free
            //node to an enabled node with the specified key/value) and returns the index of what was the last
            //free index
            size_t node_index = free_index;
            free_index = nodes[free_index].left_index;
            Node& n = nodes[node_index];
            n.reset_and_enable(key, value);
            return node_index;
        }
        virtual int insert_at_leaf(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                                  size_t& subtree_root_index,
                                  key_type const& key,
                                  value_type const& value,
                                  bool& found_key)
        {
            if (subtree_root_index == 0) {
                //key not found
                subtree_root_index = procure_node(key, value);
            } else {
                //parent was not a leaf
                //keep going down to the base of the tree
                Node& subtree_root = nodes[subtree_root_index];
                ++nodes_visited;
                switch (compare(key, subtree_root.key)) {
                case -1:
                    //key is less than subtree root's key
                    nodes_visited = insert_at_leaf(nodes_visited, subtree_root.left_index, key, value, found_key);
                    if ( ! found_key) {
                        //given key is unique to the tree, so a new node was added
                        subtree_root.num_children++;
                        subtree_root.update_height(nodes);
                    }
                    break;
                case 1:
                    //key is greater than subtree root's key
                    nodes_visited = insert_at_leaf(nodes_visited, subtree_root.right_index, key, value, found_key);
                    if ( ! found_key) {
                        //given key is unique to the tree, so a new node was added
                        subtree_root.num_children++;
                        subtree_root.update_height(nodes);
                    }
                    break;
                case 0:
                    //found key, replace the value
                    subtree_root.value = value;
                    found_key = true;
                    break;
                default:
                    throw std::domain_error("Unexpected compare() function return value");
                }
            }
            return nodes_visited;
        }
        void rotate_left(size_t& subtree_root_index) {
            Node& subtree_root = nodes[subtree_root_index];
            size_t right_child_index = subtree_root.right_index;
            Node& right_child = nodes[right_child_index];

            //original root adopts the right child's left subtree
            subtree_root.right_index = right_child.left_index;
            //original root adopted a subtree (whose height did not change), so update its height
            subtree_root.update_height(nodes);

            //right child adopts original root and its children
            right_child.left_index = subtree_root_index;
            //right child (new root) adopted the original root (whose height has been updated), so update its height
            right_child.update_height(nodes);
            //since right child took the subtree root's place, it has the same number of children as the original root
            right_child.num_children = subtree_root.num_children;

            //root has new children, so update that counter (done after changing the right child's children counter
            //because that depends on the original root's counter)
            subtree_root.num_children = 0;
            if (subtree_root.left_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.left_index].num_children;
            if (subtree_root.right_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.right_index].num_children;

            //set the right child as the new root
            subtree_root_index = right_child_index;
        }
        void rotate_right(size_t& subtree_root_index) {
            Node& subtree_root = nodes[subtree_root_index];
            size_t left_child_index = subtree_root.left_index;
            Node& left_child = nodes[left_child_index];

            //original root adopts the left child's right subtree
            subtree_root.left_index = left_child.right_index;
            //original root adopted a subtree (whose height did not change), so update its height
            subtree_root.update_height(nodes);

            //left child adopts original root and its children
            left_child.right_index = subtree_root_index;
            //left child (new root) adopted the original root (whose height has been updated), so update its height
            left_child.update_height(nodes);
            //since left child took the subtree root's place, it has the same number of children as the original root
            left_child.num_children = subtree_root.num_children;

            //root has new children, so update that counter (done after changing the left child's children counter
            //because that depends on the original root's counter)
            subtree_root.num_children = 0;
            if (subtree_root.left_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.left_index].num_children;
            if (subtree_root.right_index != 0)
                subtree_root.num_children += 1 + nodes[subtree_root.right_index].num_children;

            //set the left child as the new root
            subtree_root_index = left_child_index;
        }
        int do_search(size_t nodes_visited, //starts at 0 when this function is first called (ie does not include current node visitation)
                      size_t subtree_root_index,
                      key_type const& key,
                      value_type value) const
        {
            if (subtree_root_index == 0)
                //key not found
                nodes_visited *= -1;
            else {
                Node const& subtree_root = nodes[subtree_root_index];
                ++nodes_visited;
                switch (compare(key, subtree_root.key)) {
                case -1:
                    //key is less than subtree root key
                    nodes_visited = do_search(nodes_visited, subtree_root.left_index, key, value);
                    break;
                case 1:
                    //key is greater than subtree root key
                    nodes_visited = do_search(nodes_visited, subtree_root.right_index, key, value);
                    break;
                case 0:
                    //found key
                    value = subtree_root.value;
                    break;
                default:
                    throw std::domain_error("Unexpected compare() function return value");
                }
            }
            return nodes_visited;
        }
    public:
        /*
            The constructor will allocate an array of capacity (binary
            tree) nodes. Then make a chain from all the nodes (e.g.,
            make node 2 the left child of node 1, make node 3 the left
            child of node 2, &c. this is the initial free list.
        */
        BST(size_t capacity):
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
        virtual int insert(key_type const& key, value_type const& value) {
            if (size() == capacity())
                //no more space
                return 0;
            bool found_key = false;
            return insert_at_leaf(0, root_index, key, value, found_key);
        }
        /*
            if there is an item matching key, removes the key/value-pair from the tree, stores
            it's value in value, and returns the number of probes required, V; otherwise returns -1 * V.
        */
        virtual int remove(key_type const& key, value_type& value) {
            bool found_key = false;
            return do_remove(0, root_index, key, value, found_key);
        }
        /*
            if there is an item matching key, stores it's value in value, and returns the number
            of nodes visited, V; otherwise returns -1 * V. Regardless, the item remains in the tree.
        */
        virtual int search(key_type const& key, value_type& value) {
            return do_search(0, root_index, key, value);
        }
        /*
            removes all items from the map
        */
        virtual void clear() {
            //Since I use size_t to hold the node indices, I make the node array
            //1-based, with child index of 0 indicating that the current node is a leaf
            for (size_t i = 1; i != capacity(); ++i)
                nodes[i].disable_and_adopt_free_tree(i + 1);
            free_index = 1;
            root_index = 0;
        }
        /*
            returns true IFF the map contains no elements.
        */
        virtual bool is_empty() const {
            return size() == 0;
        }
        /*
            returns the number of slots in the backing array.
        */
        virtual size_t capacity() const {
            return curr_capacity;
        }
        /*
            returns the number of items actually stored in the tree.
        */
        virtual size_t size() const {
            if (root_index == 0) return 0;
            Node const& root = nodes[root_index];
            return 1 + root.num_children;
        }
        /*
            [not a regular BST operation, but specific to this implementation]
            returns the tree's load factor: load = size / capacity.
        */
        virtual double load() const {
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
        virtual std::ostream& print(std::ostream& out) const {
            if (root_index == 0)
                return out;
            size_t num_lines = size() * 2 - 1;
            //use CDAL here so we can print really super-huge trees where the write buffer doesn't fit in memory
            CDAL<std::string> buffer_lines(100000);
            for(size_t i = 0; i <= num_lines; ++i)
                buffer_lines.push_back("");
            Node const& root = nodes[root_index];
            size_t root_line_index = 1;
            if (root.right_index) {
                root_line_index += 2 * (1 + nodes[root.right_index].num_children);
            }
            write_subtree_buffer(root_index, buffer_lines, root_line_index, 1, num_lines + 1);
            for (size_t i = 1; i <= num_lines; ++i)
                out << buffer_lines[i] << std::endl;
            return out;
        }

        virtual priority_queue<hash_utils::ClusterInventory> cluster_distribution() {
        }

        /*
            generate a random number, R, (1,size), and starting with slot zero in the backing array, find
            the R-th occupied slot; remove the item from that slot (adjusting subsequent items as necessary),
            and return its key.
        */
        virtual key_type remove_random() {
            key_type empty;
            if (size() == 0) return empty;
            size_t num_slots = capacity();
            size_t ith_node_to_delete = 1 + hash_utils::rand_i(size());
            for (size_t i = 1; i <= num_slots; ++i) {
                Node const& n = nodes[i];
                if (n.is_occupied && --ith_node_to_delete == 0) {
                    key_type key = n.key;
                    value_type val_dummy;
                    remove(key, val_dummy);
                    return key;
                    break;
                }
            }
            return empty;
        }
    };
}

#endif
