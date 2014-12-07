#include <sys/time.h>
#include <sys/resource.h>

#include "parts1_4/common/common.h"
#include "daves_hashes/hash_string_1.h"
#include "daves_hashes/hash_string_2.h"
#include "daves_hashes/hash_string_3.h"
#include "common/program_options.h"

inline size_t my_hash(std::string const& str) {
    static cop3530::hash_utils::functors::primary_hashes::hash_basic hash_functor;
    return hash_functor(str);
}

int main (int argc, char **argv)
{
    program_options::option_desc opt_descs[] = {
        {{"hash-function", required_argument}, true, "UINT", "", "Which hash function to use"},
        {{"hash-repititions", required_argument}, false, "ULONG", "1000000", "Number of times to hash each key"},
        {{0, 0, 0, 0}}
    };
    size_t num_opts = sizeof(opt_descs) / sizeof(program_options::option_desc);
    program_options::option_bag opts;
    program_options::get_option_bag(argc, argv, opt_descs, num_opts,  &opts);
    unsigned int hash_function_id;
    size_t hash_repititions;
    opts.get_opt_val("hash-function", &hash_function_id);
    opts.get_opt_val("hash-repititions", &hash_repititions);
    size_t (*hash_function)(std::string const&);
    /*
    for each key in test case's file
        compute the key's hash
    report utime and stime 
    */
    switch (hash_function_id) {
    case 0:
        //primary hash function implemented in parts 1-4
        hash_function = &my_hash;
        break;
    case 1:
        //Dave's cat hash
        hash_function = &dave_hash_1::hash;
        break;
    case 2:
        //Paul Hsieh's SuperFastHash
        hash_function = &dave_hash_2::hash;
        break;
    case 3:
        //variation on Program 14.2 from the textbook
        hash_function = &dave_hash_3::hash;
        break;
    }
    std::string key;
    size_t hash;
    while (std::cin >> key) {
        for (size_t i = 0; i != hash_repititions; ++i)
            hash = hash_function(key);
    }

    rusage my_usage;
    getrusage(RUSAGE_SELF, &my_usage);
    double utime = my_usage.ru_utime.tv_sec + my_usage.ru_utime.tv_usec / 1000000.0;
    double stime = my_usage.ru_stime.tv_sec + my_usage.ru_stime.tv_usec / 1000000.0;
    std::cout << utime << " " << stime << std::endl;
    return 0;
}
