#include <iostream>
#include <cstdlib> //cstdlib needed for rand(), srand() functions
#include <ctime> //ctime needed for time() function

template<typename WeightType>
class WeightedGraph {
private:
    class Vertex;
    struct Edge {
        //edge knows destination vertex and weight, as well as next edge
        const Vertex* dest;
        WeightType weight;
        Edge* next;
        bool is_dummy;
    };
    class Vertex {
        public:
        //knows self-value and first edge
        Edge* edge_head;
        const unsigned int value;
        Vertex(unsigned int val): value(val) {
            edge_head = new Edge{nullptr, 0, nullptr, true};
        }
        ~Vertex() {
            while (edge_head != nullptr) {
                Edge* e = edge_head;
                edge_head = e->next;
                delete e;
            }
        }
    };
    Vertex** vertices; //array of Vertex *pointers* (saves space)
    unsigned int vertex_val_bound;
public:
    WeightedGraph(int vertex_val_ubound = 50): vertex_val_bound(vertex_val_ubound) {
        vertices = new Vertex*[vertex_val_ubound];
        for (int i = 0; i != vertex_val_ubound; ++i) {
            vertices[i] = new Vertex(i);
        }
    }
    ~WeightedGraph() {
        for (int i = 0; i != vertex_val_bound; ++i) {
            delete vertices[i];
        }
        delete[] vertices;
    }
    bool is_connected(unsigned int src_vertex_val, unsigned int dst_vertex_val) {
        Vertex* v1 = vertices[src_vertex_val], *v2 = vertices[dst_vertex_val];
        for (Edge* e = v1->edge_head->next; e != nullptr; e = e->next)
            if (e->dest == v2)
                return true;

        return false;
    }
    void connect_vertices(unsigned int src_vertex_val, unsigned int dst_vertex_val, WeightType weight) {
        if (!is_connected(src_vertex_val, dst_vertex_val)) {
            Vertex* v1 = vertices[src_vertex_val], *v2 = vertices[dst_vertex_val];
            Edge* new_edge = new Edge{v2, weight, v1->edge_head->next, false};
            v1->edge_head->next = new_edge;
        }
    }
    std::ostream& print(std::ostream& out) {
        out << "{\"nodes\": [";
        bool print_comma = false;
        int vertices_printed[vertex_val_bound];
        for (int i = 0; i != vertex_val_bound; ++i) {
            vertices_printed[i] = 0;
        }
        for (int i = 0; i != vertex_val_bound; ++i) {
            Vertex* v = vertices[i];
            if (v->edge_head->next != nullptr) {
                //node contains edges
                if (vertices_printed[i] == 0) {
                    if (print_comma) out << ",";
                    print_comma = true;
                    vertices_printed[i] = 1;
                }
                out << "{\"id\":\"n" << v->value << "\", \"label\":\"" << v->value << "\", \"x\":" << i / 20 << ", \"y\":" << i % 20 << ",\"size\":1}";
                for (Edge* e = v->edge_head->next; e != nullptr; e = e->next) {
                    if (vertices_printed[e->dest->value] == 0) {
                        vertices_printed[e->dest->value] = 1;
                        out << ",";
                        out << "{\"id\":\"n" << e->dest->value << "\", \"label\":\"" << e->dest->value << "\", \"x\":" << (e->dest->value) / 20 << ", \"y\":" << (e->dest->value) % 20 << ",\"size\":1}";

                    }
                }
            }
        }
        out << "], \"edges\": [";
        print_comma = false;
        int e_id = 0;
        for (int i = 0; i != vertex_val_bound; ++i) {
            Vertex* v = vertices[i];
            for (Edge* e = v->edge_head->next; e != nullptr; e = e->next) {
                if (print_comma) out << ",";
                print_comma = true;
                out << "{\"id\":\"e" << e_id++ << "\", \"source\": \"n" << v->value << "\", \"target\": \"n" << e->dest->value << "\"}";
            }
        }
        out << "]}";
        return out;
    }
};


template<typename T>
T** alloc2d(int rows, int cols) {
    T** arr2d = new T*[rows];
    for (int i = 0; i != rows; ++i) {
        arr2d[i] = new T[cols];
    }
    return arr2d;
}

//apparently the seed value is 1
static unsigned int prevLCG = 1;

unsigned int LCG(unsigned int a = 981237, unsigned int b = 12341234, unsigned int y = RAND_MAX) {
    unsigned int lcg = (a * prevLCG + b) % y;
    prevLCG = lcg;
    return lcg;
}

void seed_LCG(unsigned int seed) {
    prevLCG = seed;
}

int ri(int max) {
    return LCG() % max;
}

int ri_buckets(int max) {
    int num_buckets = max;
    int bucket_size = RAND_MAX / num_buckets;
    int result;
    do {
        result = LCG() / bucket_size;
    } while (result >= num_buckets * bucket_size);
    return result;
}

int main()
{
    //remember to specify type, which the compiler can't deduce by dereferencing return type
    seed_LCG(time(0));
    int bound = 200;
    int **matrix = alloc2d<int>(bound, bound);
    int hist[10]; //this is *NOT* automatically initialized to 0
    for (int i = 0; i != 10; ++i) {
        hist[i] = 0;
    }
    for (int i = 0; i != 100000; ++i) {
        hist[ri_buckets(10)]++;
    }
    int max = 0;
    for (int i = 0; i != 10; ++i) {
        if (hist[i] > max) max = hist[i];
    }
    for (int i = 0; i != 10; ++i) {
        hist[i] = static_cast<float>(hist[i]) / max * 20;
        std::cout << std::string(hist[i], '*') << std::endl;
    }
    std::cout << "creating weighted adjacency matrix" << std::endl;

    int num_source_vertices = 20;
    for (int i = 0; i != num_source_vertices; ++i) {
        int from = ri_buckets(bound);
        int num_connections = ri_buckets(10);
        for (int j = 0; j != num_connections; ++j) {
            int to = ri_buckets(bound);
            matrix[from][to] = 1 + ri_buckets(10); //weight
        }
    }
    WeightedGraph<unsigned int> graph(bound);
    std::cout << "converting to graph" << std::endl;
    for (int i = 0; i != bound; ++i) {
        for (int j = 0; j != bound; ++j) {
            int weight = matrix[i][j];
            if (weight != 0) {
                graph.connect_vertices(i, j, weight);
            }
        }
    }
    graph.print(std::cout) << std::endl;
    delete matrix;
    return 0;
}
