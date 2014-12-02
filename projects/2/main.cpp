#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include "common/common.h"
#include "part1/source/open_addressing_map.h"
#include "part2/source/buckets_map.h"
#include "part3/source/open_addressing_generic_map.h"
#include "part4/source/rbst.h"
#include "part4_bonus/source/avl.h"
#include <map>
#include "common/SSLL.h"
#include "common/priority_queue.h"

double to_dbl(int i) {
    return i + (double)i / 2;
}

int randi(int max) {
    if ( ! max)
        return 0;
    return rand() % max;
}

std::string to_str(int i) {
    return std::to_string(to_dbl(i));
}

std::string rand_word_bank_string(std::vector<std::string> const& word_bank) {
    unsigned int word_index = randi(word_bank.size());
    return word_bank[word_index];
}

void read_word_file(std::string path, std::vector<std::string>* word_bank_out) {
    std::ifstream in(path);
    if (in.good()) {
        std::string word;
        while (in >> word)
            word_bank_out->push_back(word);
    } else {
        std::cerr << "You must have a file called strings.txt in this directory containing words to use when fuzzing" << std::endl;
        exit(1);
    }
}

void histogram(std::vector<size_t> const& vals, size_t bins = 20, size_t max_bar_size = 30) {
    size_t max_val = 0;
    for (auto val: vals)
        if (val > max_val) max_val = val;
    size_t bin_size = max_val / bins;
    std::vector<size_t> bin_counter(bins);
    for (auto val: vals)
        bin_counter[val / bin_size]++;
    size_t max_bin_counter = 0;
    for (auto x: bin_counter)
        if (x > max_bin_counter) max_bin_counter = x;
    size_t bin_scale = max_bin_counter / max_bar_size;
    for (auto x: bin_counter)
        std::cout << std::string(x / bin_scale, '*') << std::endl;
}

int main() {
    std::vector<std::string> word_bank;
    read_word_file("strings.txt", &word_bank);

    cop3530::HashMapOpenAddressing open_addr_map(10000);
    for (int i = 0; i != 1000; ++i)
        open_addr_map.insert(cop3530::hash_utils::rand_i(10000), 'a');
    cop3530::priority_queue<cop3530::hash_utils::ClusterInventory> cluster_pq = open_addr_map.cluster_distribution();
    while (cluster_pq.size()) {
        cop3530::hash_utils::ClusterInventory cluster = cluster_pq.get_next_item();
        std::cout << "size=" << cluster.cluster_size << ", instances=" << cluster.num_instances << std::endl;
    }
    //histogram(vals, 40, 40);
    return 0;

    cop3530::RBST<int, std::string> map(10000);
    /*for (size_t i = 0; i != 1000; ++i)
        map.insert(cop3530::hash_utils::rand_i(100000), rand_word_bank_string(word_bank));

    for (size_t i = 0; i != 500; ++i)
        map.remove_random();
*/
    for (size_t i = 0; i != 10000; ++i) {
        map.insert(i, rand_word_bank_string(word_bank));
        if (i > 1000)
            map.remove_random();
        //map.validate_integrity();
    //map.print(std::cout) << std::endl;
    }
    map.print(std::cout) << std::endl;

    /*
    for (size_t i = 0; i != 1000; ++i) {
        if ((i % 100) == 0)
            map.print(std::cout) << std::endl;

        map.validate_integrity();
    }*/
    /*
    for (size_t i = 20; i != 30; ++i) {
        map.insert(i, rand_word_bank_string(word_bank));
        map.print(std::cout) << std::endl;
    }
    for (size_t i = 0; i != 20; ++i) {
        map.insert(cop3530::hash_utils::rand_i(100), rand_word_bank_string(word_bank));
        map.print(std::cout) << std::endl;
    }*/
    return 0;
    /*
    std::vector<int> v;
    for (int i = 0; i != 20; ++i)
        v.push_back(i);
    std::random_shuffle(v.begin(), v.end());

    for (size_t i = 0; i < 20; i++) {
        std::string val;
        std::cout << "about to remove index " << v[i] << std::endl;
        //system("sleep 1");
        int nodes_visited = map.remove(v[i], val);
        std::cout << "removed " << val << " in " << nodes_visited << " tries" << std::endl;
        map.print(std::cout) << std::endl;
        //system("sleep 1");
        if (i == 5)
            break;
    }

    for (size_t i = 0; i != 20; ++i) {
        std::cout << i << std::endl;
        map.insert(i, rand_word_bank_string(word_bank));
        map.print(std::cout) << std::endl;
    }
    std::random_shuffle(v.begin(), v.end());

    for (size_t i = 0; i < 20; i++) {
        std::cout << "deleted key " << map.remove_random() << std::endl;
        map.print(std::cout) << std::endl;
    }
*/
    return 0;
    size_t capacity = 10;
	cop3530::HashMapBuckets bucket_map(10);
	cop3530::HashMapOpenAddressingGeneric<double, char> generic_open_addr(10000);
	#define mymap generic_open_addr
	srand(1337);
	std::map<int, char> stdm;
    for (int i = 0; i != 100; ++i) {
        mymap.insert(to_dbl(i), 'a');
        stdm[i] = 'a';
    }
	for (int i = 0; i != 1000; ++i) {
        int key;
        char value;
        char value2;
        switch(rand() % 2) {
        case 0:
            //insert

            key = rand() % 20;
            value = 'a' + (rand() % 26);
            mymap.insert(to_dbl(key), value);
            stdm[key] = value;
            mymap.search(to_dbl(key), value2);
            if (stdm[key] != value2)
                std::cout << "wot" << std::endl;
            /*
            std::cout << "Inserting <" << key << "," << value << ">... ";
            if (map.insert(key, value))
                std::cout << "success" << std::endl;
            else
                std::cout << "fail" << std::endl;
            break;
            */
            break;
        case 1:
            //remove
            //std::cout << "start: " << std::endl;
            //map.print(std::cout) << std::endl;
            key = rand() % 20;
            //std::cout << "Removing key: " << key << "... ";
            //bool found = map.sequential_search(key);
            value2 = stdm[key];
            if (mymap.remove(to_dbl(key), value)){
                if (value2 != value) {
                    std::cout << "no match" << std::endl;
                }
                std::cout << "value " << (stdm[key] == value ? " does " : " does not ") << " match" << std::endl;
                stdm[key] = 0;
            }
            /*if (map.remove(key, value)) {
                if ( ! found)
                    std::cout << "shouldnt have found: " << value << std::endl;
            } else {
                if (found)
                    std::cout << "should have found" << std::endl;
            }*/
            //std::cout << "end: " << std::endl;
            //map.print(std::cout) << std::endl;
            break;
        }
    }
}


