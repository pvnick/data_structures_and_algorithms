#ifndef _FUZZER_H_
#define _FUZZER_H_

#include "cdal/source/CDAL.h"
#include "ssll/source/SSLL.h"
#include "sdal/source/SDAL.h"
#include "psll/source/PSLL.h"
#include "list_tester.h"
#include <random>
#include <functional>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>
#include <math.h>

#define FUZZ_OP_START operations.push_back({__FILE__, __LINE__, [&l](rv_func rv, T& ret_item, rand_num_func rand_int) -> int {
#define FUZZ_OP_END }, __LINE__});

namespace cop3530 {
    //returns in interval [0, max)
    //todo: redo this as buckets
    int randi(int max) {
        if ( ! max)
            return 0;
        return rand() % max;
    }

    //generic fuzzer which can perform the same operations on all our lists without reverting to polymorphism
    template <typename T>
    class ListFuzzer {
    public:
        using rv_func = std::function<T()>;
    private:
        using rand_num_func = std::function<int(int)>;
        using op_func = std::function<int(rv_func, T&, rand_num_func)>;
        size_t op_counter = 0;
        struct Op {
            std::string source_code_file;
            int source_code_start_line;
            op_func go;
            int source_code_end_line;
            std::string read_op_src() const {
                std::ifstream in(source_code_file);
                std::ostringstream out;
                for (unsigned int line = 0; line <= source_code_end_line - 2; ++line) {
                    std::string str;
                    std::getline(in, str);
                    if (line >= source_code_start_line) {
                        out << str << std::endl;
                    }
                }
                return out.str();
            }
        };
        struct OpResult {
            Op op;
            std::string list_class_name;
            int op_ret_val;
            bool list_internal_integrity_intact;
            std::string new_list_contents;
            std::string error_message;
            T returned_item;
        };
        /*
            we hold operations in one of our linked list. any of the ones implemented in this project should work.
            since op_list is rebuilt everytime an operation is performed, this is a good way of checking for memory leaks
        */
        using op_list = SDAL<Op>;

        rv_func rand_val_func;

        SSLL<T> ssll_inst;
        PSLL<T> psll_inst;
        SDAL<T> sdal_inst;
        CDAL<T> cdal_inst;

        template <typename L>
        OpResult do_operation(L& l, int rand_seed, std::string const& list_class_name) {
            op_list operations;
            FUZZ_OP_START
                //test both the copy constructer and assigner
                L copied_list(l);
                l = copied_list;
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                auto rand_val = rv();
                ret_item = l.replace(rand_val, rand_int(10));
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                auto rand_val = rv();
                return l.contains(rand_val, [](const T& a, const T& b) -> bool { return a == b; });
            FUZZ_OP_END
            FUZZ_OP_START
                return l.is_empty();
            FUZZ_OP_END
            FUZZ_OP_START
                if (rand_int(500) == 1) {
                    //acutely grow then shrink the list, leaving a bunch still in the list
                    for (int i = 0; i < 1000; ++i) {
                        l.push_front(rv());
                        l.push_back(rv());
                        int sz = l.size();
                        int rand_slot = rand_int(sz);
                        l.insert(rv(), rand_slot);
                    }
                    for (int i = 0; i < 800; ++i) {
                        ret_item = ret_item + l.pop_front();
                        ret_item = ret_item + l.pop_back();
                        int sz = l.size();
                        int rand_slot = rand_int(sz);
                        ret_item = ret_item + l.remove(rand_slot);
                    }
                    return l.size();
                } else {
                    return 0;
                }
            FUZZ_OP_END
            FUZZ_OP_START
                auto rand_val = rv();
                l.insert(rand_val, 5);
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                auto rand_val = rv();
                l.push_front(rand_val);
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                auto rand_val = rv();
                l.push_back(rand_val);
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                ret_item = l.pop_front();
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                ret_item = l.pop_back();
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                l.remove(rand_int(20));
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                ret_item = l.item_at(rand_int(20));
                return l.size();
            FUZZ_OP_END
            FUZZ_OP_START
                if (rand_int(100) == 1) {
                    //dont do this very often, we want big lists
                    l.clear();
                }
                return l.size();
            FUZZ_OP_END
            int num_ops = operations.size();
            srand(rand_seed);
            int op_index = randi(num_ops);

            Op op = operations.item_at(op_index);
            std::cout << "Running operation" << std::endl;
            std::cout << "Implementation: " << list_class_name << std::endl;
            std::cout << "Source code:" << std::endl;
            std::cout << op.read_op_src();

            OpResult op_result{op, list_class_name, 1, true, "", ""};
            try {
                ListTester<L> list_tester(l);
                op_result.op_ret_val = op.go(rand_val_func, op_result.returned_item, randi);
                op_result.list_internal_integrity_intact = list_tester.validate_internal_integrity();
                op_result.new_list_contents = get_list_contents(l);
            } catch(std::exception& e) {
                op_result.error_message = e.what();
            }
            return op_result;
        }

        template<typename L>
        std::string get_list_contents(L& l) {
            std::ostringstream oss;
            l.print(oss);
            return oss.str();
        }

        bool validate_lists(std::initializer_list<OpResult> op_results) {
            auto op_results_iter = op_results.begin();
            assert(op_results_iter != op_results.end());
            OpResult previous_result = *op_results_iter;
            ++op_results_iter;
            for (; op_results_iter != op_results.end(); ++op_results_iter) {
                OpResult result = *op_results_iter;
                std::cout << "Verifying output from " << result.list_class_name << std::endl;
                if (result.op_ret_val != previous_result.op_ret_val) {
                    std::cerr << "operation return values dont match" << std::endl;
                    std::cerr << result.list_class_name << "=" << result.op_ret_val << std::endl;
                    std::cerr << previous_result.list_class_name << "=" << previous_result.op_ret_val << std::endl;
                    throw;
                } else if (result.new_list_contents != previous_result.new_list_contents) {
                    std::cerr << "operation return values dont match" << std::endl;
                    std::cerr << result.list_class_name << "=" << result.new_list_contents << std::endl;
                    std::cerr << previous_result.list_class_name << "=" << previous_result.new_list_contents << std::endl;
                    throw;
                } else if (result.returned_item != previous_result.returned_item) {
                    std::cerr << "list returned items dont match" << std::endl;
                    std::cerr << result.list_class_name << "=" << result.returned_item << std::endl;
                    std::cerr << previous_result.list_class_name << "=" << previous_result.returned_item << std::endl;
                    throw;
                } else if ( ! result.list_internal_integrity_intact) {
                    std::cerr << "internal integrity check failed" << std::endl;
                    std::cerr << result.list_class_name << std::endl;
                    throw;
                } else if (result.error_message != previous_result.error_message) {
                    std::cerr << "error messages dont match" << std::endl;
                    std::cerr << result.list_class_name << "=" << result.error_message << std::endl;
                    std::cerr << previous_result.list_class_name << "=" << previous_result.error_message << std::endl;
                    throw;
                }
                std::cout << "Test passed: ret=" << result.op_ret_val << ", contents=" << result.new_list_contents << std::endl;
                std::cout << "error_message=" << result.error_message << std::endl;
                std::cout << "--------------------" << std::endl;
                previous_result = result;
            }
            return false;
        }

    public:
        ListFuzzer() = delete;
        ListFuzzer(unsigned int rand_seed, rv_func rand_val):
            rand_val_func(rand_val)
        {}
        void test_lists() {
            int new_seed = rand();
            validate_lists({
                do_operation(ssll_inst, new_seed, "SSLL"),
                do_operation(psll_inst, new_seed, "PSLL"),
                do_operation(sdal_inst, new_seed, "SDAL"),
                do_operation(cdal_inst, new_seed, "CDAL")
            });
            ++op_counter; //this is useful for conditional breakpoints when tracking down errors
        }
    };
}

#endif
