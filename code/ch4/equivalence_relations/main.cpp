#include <iostream>
#include <cstdlib>

//separating the high-level (connectivity) problem from the low-level (union-find) problem

class uf{
public:
    virtual int find(int, int) = 0;
    virtual void unite(int, int) = 0;
};

class UF: public uf {
//equivalence-relations ADT interface
//uses a forest of trees represented by two arrays
//weighted-quick-union
private:
    class Node {
    public:
        //holds either the index of a connected level closer to the tree root, or nullptr
        Node* parent;

        bool is_grandest_parent; //analogous to dummy tail node

        //used to link the root of the smaller of two trees to root of larger of two trees
        //holds the number of branches which connect to this level
        int num_descendants;

        Node(): num_descendants(1), is_grandest_parent(true), parent(nullptr) {};
    };

    Node* find_root(Node* n) const {
        //traverses up to tree root
        if (n->is_grandest_parent)
            return n;
        else
            return find_root(n->parent);
    }

    Node* find_root(int node_index) const {
        return find_root(node_array + node_index);
    }

    void compress_path_to_root(Node* n) {
        //halves the number of links needed to get to root
        if ( ! n->is_grandest_parent) {
            while ( ! n->parent->is_grandest_parent) {
                Node* x = n->parent;
                n->parent = n->parent->parent;
                n = x;
            }
        }
    }

    Node* node_array;
public:
    UF(int N) {
        node_array = new Node[N];
    }
    int find(int p, int q) {
        //returns true IFF p and q are already connected
        //recursively traverse up the tree until we get to the root
        return find_root(p) == find_root(q);
    }
    int num_connected_nodes(int p) {
        return find_root(p)->num_descendants;
    }
    void unite(int p, int q) {
        Node* i = find_root(p);
        Node* j = find_root(q);
        //pair already united
        if (i == j)
            return;

        compress_path_to_root(i);
        compress_path_to_root(j);

        if (i->num_descendants > j->num_descendants) {
            //i takes over j
            j->parent = i;
            j->is_grandest_parent = false;
            i->num_descendants += j->num_descendants;
        } else {
            //j takes over i
            i->parent = j;
            i->is_grandest_parent = false;
            j->num_descendants += i->num_descendants;
        }

        std::cout << " " << p << " " << q << std::endl;
    }
};


int main(int argc, const char* argv[])
{
    //equivalence-relations ADT client
    //given multiple pairs of numbers, print the pair if they were not yet connected
    //to connect, we're building trees whose roots end up getting connected
    int p, q, N = 10; //atoi(argv[1]);
    UF info(N);
    while (std::cin >> p >> q) {
        std::cout << p << " had " << info.num_connected_nodes(p) << " connected nodes" << std::endl;
        std::cout << q << " had " << info.num_connected_nodes(q) << " connected nodes" << std::endl;
        info.unite(p, q);
        std::cout << "now " << p << " has " << info.num_connected_nodes(p) << " connected nodes" << std::endl;
        std::cout << "now " << q << " has " << info.num_connected_nodes(q) << " connected nodes" << std::endl;
    }
    return 0;
}
