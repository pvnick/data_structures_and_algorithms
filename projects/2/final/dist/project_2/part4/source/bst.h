#ifndef _BST_H_
#define _BST_H_

#include <cstdlib>
#include <sstream>
#include "../../common/CDAL.h"
#include "../../common/common.h"
#include "../../common/priority_queue.h"

namespace cop3530 {
    template<typename key_type,
             typename value_type>
    class BST {
    protected: //let RBST and AVL inherit everything
        typedef hash_utils::ClusterInventory ClusterInventory;
        typedef hash_utils::Key<key_type> Key;
        typedef hash_utils::Value<value_type> Value;
        struct Node;
        typedef Node* link;
        struct Node {
            Key key;
            Value value;
            size_t num_children;
            size_t left_index;
            size_t right_index;
            size_t height; //height tracking coded in this class, but not used (for AVL, which is this class with self-balancing)
            bool is_occupied;
            size_t validate_children_count_recursive(Node* nodes) {
                //this function is for debugging purposes, does recursive traversal to find the correct number of children
                size_t child_count = 0;
                if (left_index)
                    child_count += 1 + nodes[left_index].validate_children_count_recursive(nodes);
                if (right_index)
                    child_count += 1 + nodes[right_index].validate_children_count_recursive(nodes);
                if (child_count != num_children) {
                    std::ostringstream msg;
                    msg << "Manually counted children, " << child_count << ", different than child count, " << num_children;
                    throw std::logic_error(msg.str());
                }
                return child_count;
            }
            size_t get_height_recursive(Node* nodes) {
                //this function is for debugging purposes, does recursive traversal to find the correct height
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
                if (_DEBUG_) {
                    size_t calculated_height = get_height_recursive(nodes);
                    if (calculated_height != height) {
                        std::ostringstream msg;
                        msg << "Manually calculated height, " << calculated_height << ", different than tracked height, " << height;
                        throw std::logic_error(msg.str());
                    }
                }
            }
            void disable_and_adopt_free_tree(size_t free_index) {
                is_occupied = false;
                height = 0;
                num_children = 0;
                right_index = 0;
                left_index = free_index;
            }
            void reset_and_enable(Key const& new_key, Value const& new_value) {
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
                              Key const& key,
                              Value& value,
                              bool& found_key)
        {
            if (subtree_root_index != 0) {
                Node& subtree_root = nodes[subtree_root_index];
                ++nodes_visited;
                //keep going down to the base of the tree
                if (key < subtree_root.key) {
                    nodes_visited = do_remove(nodes_visited, subtree_root.left_index, key, value, found_key);
                    if (found_key) {
                        //found the desired node and delete it
                        subtree_root.num_children--;
                        //left child changed, so recompute subtree height
                        subtree_root.update_height(nodes);
                    }
                } else if (key > subtree_root.key) {
                    nodes_visited = do_remove(nodes_visited, subtree_root.right_index, key, value, found_key);
                    if (found_key) {
                        //found the desired node and delete it
                        subtree_root.num_children--;
                        //right child changed, so recompute subtree height
                        subtree_root.update_height(nodes);
                    }
                } else if (key == subtree_root.key) {
                    //found key, remove the node
                    found_key = true;
                    value = subtree_root.value;
                    remove_node(subtree_root_index);
                } else {
                    throw std::logic_error("Unexpected compare result");
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
            oss << "[" << subtree_root.key.raw() << "]";
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
        size_t procure_node(Key const& key, Value const& value) {
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
                                  Key const& key,
                                  Value const& value,
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
                if (key < subtree_root.key) {
                    nodes_visited = insert_at_leaf(nodes_visited, subtree_root.left_index, key, value, found_key);
                    if ( ! found_key) {
                        //given key is unique to the tree, so a new node was added
                        subtree_root.num_children++;
                        subtree_root.update_height(nodes);
                    }
                } else if (key > subtree_root.key) {
                    nodes_visited = insert_at_leaf(nodes_visited, subtree_root.right_index, key, value, found_key);
                    if ( ! found_key) {
                        //given key is unique to the tree, so a new node was added
                        subtree_root.num_children++;
                        subtree_root.update_height(nodes);
                    }
                } else if (key == subtree_root.key) {
                    //found key, replace the value
                    subtree_root.value = value;
                    found_key = true;
                } else {
                    throw std::logic_error("Unexpected compare result");
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
                      Key const& key,
                      Value& value,
                      bool& found_key) const
        {
            if (subtree_root_index != 0) {
                Node const& subtree_root = nodes[subtree_root_index];
                ++nodes_visited;
                if (key < subtree_root.key) {
                    nodes_visited = do_search(nodes_visited, subtree_root.left_index, key, value, found_key);
                } else if (key > subtree_root.key) {
                    nodes_visited = do_search(nodes_visited, subtree_root.right_index, key, value, found_key);
                } else if (key == subtree_root.key) {
                    //found key, replace the value
                    value = subtree_root.value;
                    found_key = true;
                } else {
                    throw std::logic_error("Unexpected compare result");
                }
            }
            return nodes_visited;
        }
        void prepare_cluster_distribution(size_t subtree_root_index,
                                          size_t curr_height, //includes the height of the current node, ie assumes current node exists
                                          size_t cluster_counter[])
        {
            Node const& subtree_root = nodes[subtree_root_index];
            if ( ! subtree_root.left_index && ! subtree_root.right_index)
                //at a leaf node
                cluster_counter[curr_height]++;
            else {
                if (subtree_root.left_index)
                    prepare_cluster_distribution(subtree_root.left_index, curr_height + 1, cluster_counter);
                if (subtree_root.right_index)
                    prepare_cluster_distribution(subtree_root.right_index, curr_height + 1, cluster_counter);
            }
        }

        void remove_ith_node_inorder(size_t& subtree_root_index,
                                     size_t& ith_node_to_delete,
                                     Key& key)
        {
            Node& subtree_root = nodes[subtree_root_index];
            if (subtree_root.left_index)
                remove_ith_node_inorder(subtree_root.left_index, ith_node_to_delete, key);
            if (ith_node_to_delete == 0)
                //deleted node in child subtree; nothing more to do
                return;
            if (--ith_node_to_delete == 0) {
                //delete the current node
                value_type dummy_val;
                remove(subtree_root.key.raw_copy(), dummy_val);
                key = subtree_root.key;
                return;
            }
            if (subtree_root.right_index)
                remove_ith_node_inorder(subtree_root.right_index, ith_node_to_delete, key);
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
                return -1 * size();
            bool found_key = false;
            Key k(key);
            Value v(value);
            int nodes_visited = insert_at_leaf(0, root_index, k, v, found_key);
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            return nodes_visited;
        }
        /*
            if there is an item matching key, removes the key/value-pair from the tree, stores
            it's value in value, and returns the number of probes required, V; otherwise returns -1 * V.
        */
        virtual int remove(key_type const& key, value_type& value) {
            if (is_empty())
                return 0;
            bool found_key = false;
            Key k(key);
            Value v(value);
            int nodes_visited = do_remove(0, root_index, k, v, found_key);
            if (_DEBUG_)
                this->nodes[this->root_index].validate_children_count_recursive(this->nodes);
            if (found_key)
                value = v.raw_copy();
            return found_key ? nodes_visited : -1 * nodes_visited;
        }
        /*
            if there is an item matching key, stores it's value in value, and returns the number
            of nodes visited, V; otherwise returns -1 * V. Regardless, the item remains in the tree.
        */
        virtual int search(key_type const& key, value_type& value) {
            if (is_empty())
                return 0;
            bool found_key = false;
            Key k(key);
            Value v(value);
            int nodes_visited = do_search(0, root_index, k, v, found_key);
            if (found_key)
                value = v.raw_copy();
            return found_key ? nodes_visited : -1 * nodes_visited;
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
            if (is_empty())
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

        /*
            returns a list indicating the number of leaf nodes at each height (since the RBST doesn't exhibit
            true clustering, but can have degenerate branches).
        */
        virtual priority_queue<hash_utils::ClusterInventory> cluster_distribution() {
            //use an array to count cluster instances, then feed those to a priority queue and return it.
            priority_queue<ClusterInventory> cluster_pq;
            if (is_empty()) return cluster_pq;
            size_t max_height = nodes[root_index].height;
            size_t cluster_counter[max_height + 1];
            for (size_t i = 0; i <= max_height; ++i)
                cluster_counter[i] = 0;
            prepare_cluster_distribution(root_index, 1, cluster_counter);
            for (size_t i = 1; i <= max_height; ++i)
                if (cluster_counter[i] > 0) {
                    ClusterInventory cluster{i, cluster_counter[i]};
                    cluster_pq.add_to_queue(cluster);
                }
            return cluster_pq;
        }

        /*
            generate a random number, R, (1,size), and starting with the root (node 1), do an in-order
            traversal to find the R-th occupied node; remove that node (adjusting its children accordingly),
            and return its key.

            ***XXX: this likely contains a bug when using const char* keys in that we'll be returning a dangling pointer!!!***
        */
        virtual key_type remove_random() {
            if (size() == 0) throw std::logic_error("Cant remove from an empty map");
            size_t ith_node_to_delete = 1 + rand_i(size());
            Key key;
            remove_ith_node_inorder(root_index, ith_node_to_delete, key);
            key_type ret = key.raw_copy();
            return ret;
        }
    };
}

#endif
