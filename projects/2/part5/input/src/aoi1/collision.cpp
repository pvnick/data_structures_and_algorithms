#include <sys/time.h>
#include <sys/resource.h>

#include "parts1_4/common/common.h"
#include "parts1_4/part3/bucket/source/buckets_map_generic.h"
#include "parts1_4/common/priority_queue.h"
#include "daves_hashes/hash_string_1.h"
#include "daves_hashes/hash_string_2.h"
#include "daves_hashes/hash_string_3.h"
#include "common/program_options.h"

struct hash_functor {
    static size_t (*hash_function)(std::string const&);
    size_t operator()(std::string const& key) {
        return hash_function(key);
    }
};

inline size_t my_hash(std::string const& str) {
    static cop3530::hash_utils::functors::primary_hashes::hash_basic hash_functor;
    return hash_functor(str);
}

//the following may get overridden by the user-specified hash function
size_t (*hash_functor::hash_function)(std::string const&) = &my_hash;

int main (int argc, char **argv)
{
    program_options::option_desc opt_descs[] = {
        {{"hash-function", required_argument}, true, "UINT", "", "Which hash function to use"},
        {{"map-capacity", required_argument}, true, "ULONG", "", "Map table capacity"},
        {{0, 0, 0, 0}}
    };
    size_t num_opts = sizeof(opt_descs) / sizeof(program_options::option_desc);
    program_options::option_bag opts;
    program_options::get_option_bag(argc, argv, opt_descs, num_opts,  &opts);
    unsigned int hash_function_id;
    size_t map_capacity;
    opts.get_opt_val("hash-function", &hash_function_id);
    opts.get_opt_val("map-capacity", &map_capacity);
    switch (hash_function_id) {
    case 0:
        //primary hash function implemented in parts 1-4
        hash_functor::hash_function = &my_hash;
        break;
    case 1:
        //Dave's cat hash
        hash_functor::hash_function = &dave_hash_1::hash;
        break;
    case 2:
        //Paul Hsieh's SuperFastHash
        hash_functor::hash_function = &dave_hash_2::hash;
        break;
    case 3:
        //variation on Program 14.2 from the textbook
        hash_functor::hash_function = &dave_hash_3::hash;
        break;
    }
    /*
    for each key in test case's file
      compute the key's hash
      for each table capacity to be considered
        compute index from key for tables 
        increment count of keys hashing to that index for that table's capacity
    for each table capacity to be considered
      output the table's capacity
      for X = 1 to table size
        if at least one index has exactly X collisions
          output: X and the number of indices that has exactly X collisions
    this can all be handled with the generic bucket map's cluster_distribution function
    */
    cop3530::HashMapBucketsGeneric<std::string, bool, hash_functor> map(map_capacity);
    std::string key;
    while (std::cin >> key) {
        map.insert(key, true);
    }
    
    auto cluster_pq = map.cluster_distribution();
    while ( ! cluster_pq.empty()) {
        auto cluster_type = cluster_pq.get_next_item();
        std::cout << cluster_type.cluster_size << " " << cluster_type.num_instances << std::endl;
    }
    return 0;
}
