#include <iostream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parts1_4/part1/source/open_addressing_map.h"
#include "parts1_4/part2/source/buckets_map.h"
#include "parts1_4/part3/bucket/source/buckets_map_generic.h"
#include "parts1_4/part3/open_addressing/source/open_addressing_generic_map.h"
#include "parts1_4/part4/source/rbst.h"
#include "parts1_4/part4bonus/source/avl.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

struct option_desc {
    struct option gnu_opt;
    bool mandatory_opt;
    const char* value_name;
    bool user_provided;
};

void show_usage(std::string const& executable_name, option_desc options[], size_t num_options) {
    std::cout << "Usage: " << executable_name;
    for (size_t i = 0; i != num_options; ++i) {
        option_desc opt_desc = options[i];
        if (opt_desc.gnu_opt.name) {
            std::cout << " ";
            if ( ! opt_desc.mandatory_opt)
                std::cout << "[";
            std::cout << "--" << opt_desc.gnu_opt.name;
            if (opt_desc.value_name)
                std::cout << " " << opt_desc.value_name;
            if ( ! opt_desc.mandatory_opt)
                std::cout << "]";
        }
    }
    std::cout << std::endl;
}

int main (int argc, char **argv)
{
    int c;
    std::string input_file;
    unsigned int hash_function_index;
    size_t map_capacity;
    bool input_file_opt_seen, hash_function_opt_seen, map_capacity_opt_seen;
    option foobar = {"input-file", required_argument, 0, 'i'};
    option_desc options[] = {
        {{"input-file", required_argument, 0, 'i'}, true, "FILE"}, 
        {{"hash-function", required_argument, 0, 'h'}, true, "UINT"}, 
        {{"map-capacity", required_argument, 0, 'c'}, true, "ULONG"}, 
        {{0, 0, 0, 0}, 0}
    };
    size_t num_opts = sizeof(options) / sizeof(option_desc);
    option gnu_long_opts[num_opts];
    size_t i = 0;
    for (option_desc opt_desc: options)
        gnu_long_opts[i++] = opt_desc.gnu_opt;

    while (1) {
        // getopt_long stores the option index here.
        int option_index = 0;

        try {
            c = getopt_long(argc, 
                            argv, 
                            "i:h:c:",
                            gnu_long_opts, 
                            &option_index);
            // Detect the end of the options.
            if (c == -1)
                break;

            //remember that this argument was provided
            options[option_index].user_provided = true;

            switch (c) {
            case 'i':
                input_file = std::string(optarg);
                break;
            case 'h':
                hash_function_index = std::stoi(optarg);
                break;
            case 'c':
                map_capacity = std::stol(optarg);
                break;
            case '?':
                // getopt_long already printed an error message. 
                break;
            default:
                abort ();
            }
        } catch (std::invalid_argument) {
            option_desc bad_opt = options[option_index];
            std::cerr << "Invalid argument provided for " << bad_opt.gnu_opt.name << std::endl;
            show_usage(argv[0], options, num_opts);
            return 1;
        } catch (std::exception) {
            option_desc bad_opt = options[option_index];
            std::cerr << "Unexpected exception when parsing argument " << bad_opt.gnu_opt.name << std::endl;
            show_usage(argv[0], options, num_opts);
            return 1;
        }
    }
    std::cout << "hello world" << std::endl;
    for (option_desc opt_desc: options)
        if (opt_desc.mandatory_opt && opt_desc.user_provided == false) {
            std::cerr << "You must specify --" << opt_desc.gnu_opt.name << std::endl;
            show_usage(argv[0], options, num_opts);
            return 1;
        }
    std::cout << "goodbye world" << std::endl;
    return 0;
}
