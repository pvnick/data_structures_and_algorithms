#ifndef _UNIT_TEST_UTILS_H_
#define _UNIT_TEST_UTILS_H_

#include <iostream>
#include <string>
#include <fstream>

namespace cop3530 {
    namespace unit_test_utils {
        inline std::string guid() {
            std::string ret = "";
            for (size_t i = 0; i != 32; ++i) {
                size_t rnd = cop3530::rand_i(16);
                if (rnd < 10)
                    ret += std::string(1, '0' + rnd);
                else
                    ret += std::string(1, 'A' + rnd - 10);
            }
            return ret;
        }
        inline std::string get_tmp_filename() {
            return std::string("/tmp/") + guid() + std::string(".out");
        }
        inline void delete_file(std::string file_path) {
            system((std::string("rm ") + file_path + std::string(" 2>&1 >> /tmp/debug")).c_str());
        }
        inline std::string shell_cmd(std::string cmd) {
            std::string shell_script_file = get_tmp_filename();
            std::string output_file = get_tmp_filename();
            std::ofstream shell_script_out(shell_script_file);
            shell_script_out << "#!/bin/sh" << std::endl << cmd << std::endl;
            shell_script_out.close();
            system((std::string("chmod +x ") + shell_script_file).c_str());
            system((shell_script_file + std::string(" > ") + output_file).c_str());
            std::ifstream read_output(output_file);
            std::ostringstream oss;
            std::string tmp;
            while (std::getline(read_output, tmp)) {
                oss << tmp;
                if ( ! read_output.eof())
                    oss << "\n";
            }
            delete_file(output_file);
            delete_file(shell_script_file);
            return oss.str();
        }
    }
}

#endif // _UNIT_TEST_UTILS_H_
