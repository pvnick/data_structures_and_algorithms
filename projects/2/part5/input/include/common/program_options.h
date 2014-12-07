#ifndef _PART5_PROGRAM_OPTIONS_H_
#define _PART5_PROGRAM_OPTIONS_H_

/*
    program_options.h
    Author: Paul Nickerson
    Description: A rudimentary method for extracting command-line options.
    Essentially a wrapper for GNU getopts, with support for showing correct CLI usage.
    This should suffice for all programs written in fulfillment of project 2, part 5.
*/


#include <iostream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdexcept>

namespace program_options {
    struct convert_opt_val_functor {
        void operator()(std::string const& val, int* out) {
            int ret = std::stoi(val);
            if (std::to_string(ret) != val)
                throw std::invalid_argument(val + " is not a valid integer");
            *out = ret;
        }
        void operator()(std::string const& val, size_t* out) {
            size_t ret = std::stol(val);
            if (std::to_string(ret) != val)
                throw std::invalid_argument(val + " is not a valid long");
            *out = ret;
        }
        void operator()(std::string const& val, double* out) {
            size_t ret = std::stod(val);
            if (std::to_string(ret) != val)
                throw std::invalid_argument(val + " is not a valid double");
            *out = ret;
        }
        void operator()(std::string const& val, std::string* out) {
            *out = val;
        }
    };

    struct option_desc {
        //specifiable members
        struct option gnu_opt;
        bool mandatory_opt;
        std::string value_name;

        //internal members
        std::string opt_name; //arg_name provided as const char* to get_opt, we convert to string to avoid dangling pointer
        bool user_provided; //did the user supply this arg?
        std::string opt_val; //we convert the optarg const char* to a string to avoid dangling pointer
        convert_opt_val_functor convert_arg;
        bool is_placeholder;

        template<typename T>
        T get() {
            T out;
            convert_arg(opt_val, &out);
            return out;
        }
    };

    struct option_bag {
        option_desc* options;
        size_t num_options;
        std::string executable_name;

        template<typename T>
        T get_opt_val(std::string const& opt_name) {
            for (size_t i = 0; i != num_options; ++i) {
                option_desc& desc = options[i];
                if (desc.opt_name == opt_name) {
                    try {
                        return desc.get<T>();
                    } catch (std::invalid_argument& e) {
                        std::cerr << "Invalid argument provided for " << desc.opt_name << ":" << std::endl;
                        std::cerr << e.what() << std::endl;
                        show_usage();
                        exit(1);
                    } catch (std::exception& e) {
                        std::cerr << "Unexpected exception when parsing argument " << desc.opt_name << ":" << std::endl;
                        std::cerr << e.what() << std::endl;
                        show_usage();
                        exit(1);
                    }
                }
            }
            throw std::invalid_argument(std::string("Option not found: ") + opt_name);
        }

        void show_usage() {
            std::cout << "Usage: " << executable_name;
            for (size_t i = 0; i != num_options; ++i) {
                option_desc opt_desc = options[i];
                if ( ! opt_desc.opt_name.empty()) {
                    std::cout << " ";
                    if ( ! opt_desc.mandatory_opt)
                        std::cout << "[";
                    std::cout << "--" << opt_desc.opt_name;
                    if ( ! opt_desc.value_name.empty())
                        std::cout << " " << opt_desc.value_name;
                    if ( ! opt_desc.mandatory_opt)
                        std::cout << "]";
                }
            }
            std::cout << std::endl;
        }
    };

    void get_option_bag(int argc, char **argv, option_desc opt_descs[], size_t num_opts, std::string const& getopt_optstring, option_bag* bag_ptr) {
        int c;
        *bag_ptr = {
            opt_descs,
            num_opts,
            argv[0]
        };
        option_bag& bag = *bag_ptr;
        option gnu_long_opts[num_opts];
        for (size_t i = 0; i != num_opts; ++i) {
            option_desc& opt_desc = bag.options[i];
            if (opt_desc.gnu_opt.name) {
                opt_desc.opt_name = std::string(opt_desc.gnu_opt.name);
                opt_desc.is_placeholder = false;
            } else {
                opt_desc.opt_name = "";
                opt_desc.is_placeholder = true;
            }
            gnu_long_opts[i] = opt_desc.gnu_opt;
        }

        while (1) {
            // getopt_long stores the option index here.
            int option_index = 0;

            c = getopt_long(argc,
                            argv,
                            getopt_optstring.c_str(),
                            gnu_long_opts,
                            &option_index);
            // Detect the end of the options.
            if (c == -1)
                break;

            option_desc& option = bag.options[option_index];
            option.user_provided = true;
            option.opt_val = std::string(optarg);
        }
        for (size_t i = 0; i != num_opts; ++i) {
            option_desc& opt_desc = bag.options[i];
            if (opt_desc.mandatory_opt && opt_desc.user_provided == false) {
                std::cerr << "You must specify --" << opt_desc.opt_name << std::endl;
                bag.show_usage();
                exit(1);
            }
        }
    }
}

#endif
