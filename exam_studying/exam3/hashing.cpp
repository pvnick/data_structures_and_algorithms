#include <iostream>

template <class Key, class Value>
class ST {
private:
    struct KVPair {
        Key key;
        Value value;
    };
    KVPair nullitem;
    struct Node {
        KVPair item;
        Node* next;
        Node(KVPair item, Node* next): item(item), next(next) {}
    };
    typedef Node* link;
    int N, M;
    link* map_array;
    KVPair searchR(link root, Key key) {
        while (root != nullptr) {
            if (root->item.key == key)
                return root->item;
            else
                root = root->next;
        }
        return nullitem;
    }
    size_t hash(Key key) {

    }
public:
    ST() {
        N = 0;
        M = 251;
        map_array = new link[M];
        for (size_t i = 0; i != M; ++i)
            map_array[i] = nullptr;
    }
    ~ST() {
        for (size_t i = 0; i != M; ++i)
            delete map_array[i];
        delete map_array;
    }

};

int main()
{
    ST<unsigned int, char> map();
    map.insert()
    return 0;
}
