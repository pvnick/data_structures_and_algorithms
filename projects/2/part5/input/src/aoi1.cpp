#include "parts1_4/part1/source/open_addressing_map.h"
#include "parts1_4/part2/source/buckets_map.h"
#include "parts1_4/part3/bucket/source/buckets_map_generic.h"
#include "parts1_4/part3/open_addressing/source/open_addressing_generic_map.h"
#include "parts1_4/part4/source/rbst.h"
#include "parts1_4/part4bonus/source/avl.h"
#include "common/program_options.h"

int main (int argc, char **argv)
{
    program_options::option_desc opt_descs[] = {
        {{"field", required_argument, 0, 'f'}, false, "FILE", "", "If provided, specified which field to extract from stdin (presumed to be lines)"},
        {{"hash-function", required_argument, 0, 'h'}, true, "UINT", 0, "", },
        {{"map-capacity", required_argument, 0, 'c'}, true, "ULONG", 0, "", },
        {{0, 0, 0, 0}}
    };
    size_t num_opts = sizeof(opt_descs) / sizeof(program_options::option_desc);
    program_options::option_bag opts;
    program_options::get_option_bag(argc, argv, opt_descs, num_opts, "h:c:",  &opts);
    size_t hash_function;
    opts.get_opt_val("hash-function", &hash_function);
    std::cout << hash_function << std::endl;
    std::cout << "goodbye world" << std::endl;
    return 0;
}
