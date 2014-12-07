#include "common/program_options.h"
#include <sstream>

/*
    Note: Input should be piped to this program using the following command:
    cat input_file | sed 's/:/ /g' | sed 's/  / . /g' | extract_field opts
*/
int main (int argc, char **argv)
{
    program_options::option_desc opt_descs[] = {
        {{"field-to-extract", required_argument, 0, 'f'}, false, "INT", "1", "Which field to extract per line (first field in line = 1)"},
        {{0, 0, 0, 0}}
    };
    size_t num_opts = sizeof(opt_descs) / sizeof(program_options::option_desc);
    program_options::option_bag opts;
    program_options::get_option_bag(argc, argv, opt_descs, num_opts, "f:d:",  &opts);
    size_t field_to_extract;
    opts.get_opt_val("field-to-extract", &field_to_extract);
    std::string field_str;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        for (size_t field_id = 1; iss >> field_str; field_id++) {
            if (field_id == field_to_extract)
                std::cout << field_str << std::endl;
        }
    }
    return 0;
}
