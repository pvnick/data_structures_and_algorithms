#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include "input/common/common.h"
#include "input/part1/source/open_addressing_map.h"
#include "input/part2/source/buckets_map.h"
#include "input/part3/open_addressing/source/open_addressing_generic_map.h"
#include "input/part3/bucket/source/buckets_map_generic.h"
#include "input/part4/source/rbst.h"
#include "input/part4_bonus/source/avl.h"
#include <map>
#include "input/common/SSLL.h"
#include "input/common/priority_queue.h"

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
    cop3530::HashMapBucketsGeneric<std::string, int> map(1000);
    for (size_t i = 0; i != 100000; ++i) {
        map.insert(std::to_string(cop3530::hash_utils::rand_i(1000000)), i);
    }
    map.print(std::cout) << std::endl;

    cop3530::priority_queue<cop3530::hash_utils::ClusterInventory> cluster_pq = map.cluster_distribution();
    while (cluster_pq.size()) {
        cop3530::hash_utils::ClusterInventory cluster = cluster_pq.get_next_item();
        std::cout << cluster << std::endl;
    }
    //histogram(vals, 40, 40);
    return 0;

}
