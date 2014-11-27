#include "fuzzer.h"
#include <iostream>
#include <fstream>
#include <vector>

std::string rand_word_bank_string(std::vector<std::string> const& word_bank) {
    unsigned int word_index = cop3530::randi(word_bank.size());
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

int main() {
    using cop3530::randi;
    std::vector<std::string> word_bank;
    read_word_file("strings.txt", &word_bank);
    unsigned int rand_seed = 1337; //hax
    cop3530::ListFuzzer<std::string> fuzzer(rand_seed,
                       std::function<std::string()>([&word_bank]() -> std::string { return rand_word_bank_string(word_bank); }));
    while(1)
        fuzzer.test_lists();
    return 0;
}
