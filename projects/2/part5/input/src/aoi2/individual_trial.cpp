#include "daves_hashes/hash_string_2.h"
#include "common/program_options.h"
#include "parts1_4/part3/open_addressing/source/open_addressing_generic_map.h"
#include <fstream>

struct Hash2Functor {
    size_t operator()(std::string const& key) {
        return dave_hash_2::hash(key);
    }
};

int main (int argc, char **argv)
{
    program_options::option_desc opt_descs[] = {
        {{"capacity", required_argument}, true, "ULONG", "", "map capacity"},
        {{"target-load", required_argument}, true, "DOUBLE", "", "target load factor"},
        {{"input-file", required_argument}, true, "FILE", "", "file containing the data set"},
        {{"input-file-unique", required_argument}, true, "FILE", "", "corresponding file containing the unique data set"},
        {{"output-dir", required_argument}, true, "DIR", "", "where to save the results"},
        {{0, 0, 0, 0}}
    };
    size_t num_opts = sizeof(opt_descs) / sizeof(program_options::option_desc);
    program_options::option_bag opts;
    program_options::get_option_bag(argc, argv, opt_descs, num_opts,  &opts);
    size_t M;
    double target_load;
    std::string input_file;
    std::string input_file_unique;
    std::string output_dir;
    opts.get_opt_val("capacity", &M);
    opts.get_opt_val("target-load", &target_load);
    opts.get_opt_val("input-file", &input_file);
    opts.get_opt_val("input-file-unique", &input_file_unique);
    opts.get_opt_val("output-dir", &output_dir);
    std::string key;
    cop3530::HashMapOpenAddressingGeneric<std::string, size_t, Hash2Functor, cop3530::hash_utils::functors::secondary_hashes::linear_probe> map(M);
    /*
        // starting from the beginning of the trial data set
        while (load factor < target load factor) // See note below
          insert next item from data set

        while (items remain in data set)
          remove random item from table
          insert next item from data set tracking #probes required

        // starting from the beginning of the corresponding unique data set
        // note: we want to search for each item exactly once.
        while (items remain in the corresponding unique data set)
          search for next item from unique data set tracking success/failure/#probes required

        // save the results
        output the table's capacity
        output the table's size
        output the number of items in the trial data set
        output the number of items in the unique data set
        output insert probe count distribution
        output search success probe count distribution
        output search failure probe count distribution
    */
    size_t value = 0;
    std::ifstream in(input_file);
    std::ifstream in_uniq(input_file_unique);
    std::ofstream table_capacity_out(output_dir + "/table_capacity");
    std::ofstream table_size_out(output_dir + "/table_size");
    std::ofstream trial_data_set_length_out(output_dir + "/trial_data_set_length");
    std::ofstream unique_data_set_length_out(output_dir + "/unique_data_set_length");
    std::ofstream probes_required_out(output_dir + "/probes_required");
    std::ofstream success_probes_required_out(output_dir + "/success_probes_required");
    std::ofstream failure_probes_required_out(output_dir + "/failure_probes_required");
    size_t trial_data_set_length = 0;
    size_t unique_data_set_length = 0;
    for (; (in >> key) && map.load() < target_load; ++value) {
        map.insert(key, value);
    }
    for (; in >> key; ++value) {
        map.remove_random();
        size_t probes_required = map.insert(key, value);
        probes_required_out << probes_required << std::endl;
    }
    while (in_uniq >> key) {
        int search_probes = map.search(key, value);
        probes_required_out << std::abs(search_probes) << std::endl;
        if (search_probes < 0)
            failure_probes_required_out << std::abs(search_probes) << std::endl;
        else
            success_probes_required_out << std::abs(search_probes) << std::endl;
    }
    size_t table_size = map.size();
    size_t table_capacity = M;
    table_capacity_out << table_capacity << std::endl;
    table_size_out << table_size << std::endl;
    trial_data_set_length_out << trial_data_set_length << std::endl;
    unique_data_set_length_out << unique_data_set_length << std::endl;


    return 0;
}
