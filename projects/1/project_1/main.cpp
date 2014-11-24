#include "test/unit_tests.h"
#include "src/CDAL.h"
#include "src/SSLL.h"
#include "src/SDAL.h"
#include "src/PSLL.h"
#include "test/fuzzer.h"
#include <sstream>
#include <fstream>


unsigned int word_bank_size;
static std::string* word_bank = nullptr;
using cop3530::randi;

std::string rand_string() {
    unsigned int word_index = randi(word_bank_size);
    return word_bank[word_index];
}

void read_word_file(std::string path) {
    if (word_bank != nullptr) {
        delete word_bank;
        word_bank = nullptr;
    }
    std::ifstream in;
    std::string word;
    in.open(path);
    for (word_bank_size = 0; in >> word; ++word_bank_size);
    in.close();
    in.open(path);
    word_bank = new std::string[word_bank_size];
    for (unsigned int word_index = 0; in >> word_bank[word_index]; ++word_index);
}

bool same_string( const std::string& a, const std::string& b ) { // argument for contains()
    return (a == b);
}

char rand_char() {
    return static_cast<char>(randi(256));
}

bool same_char( const char& a, const char& b ) { // argument for contains()
    return (a == b);
}


typedef cop3530::CDAL<std::string> listtype;

void fuzz() {
    read_word_file("/home/pvnick/data_structures_and_algorithms/projects/1/misc/AV1611Bible.txt");

    listtype l;
    auto rand_int_lt_10 =  [](){return static_cast<int>(rand()) % 10;};
    auto int_eq = [](const int& a, const int &b) -> bool{return a==b;};
    auto rv = rand_string; //rand_int_lt_10;
    auto rand_int = randi;
    int ret_item;
    //acutely grow then shrink the list, leaving a bunch still in the list
    for (int i = 0; i < 500; ++i) {
        l.push_front(rv());
        l.push_back(rv());
        int sz = l.size();
        int rand_slot = rand_int(sz);
        l.insert(rv(), rand_slot);
    }


    for (listtype::iterator strt = l.begin(); strt != l.end(); ++strt) {
        *strt += std::string("    ") + rand_string();
    }

    const listtype cl = l;
    for (listtype::const_iterator strt = cl.begin(); strt != cl.end(); ++strt) {
        std::cout << *strt << std::endl;
    }

    l[32] = cl[3];


    decltype(l) l2(l);
    for (int i = 0; i < 100; ++i) {
        l.pop_front();
        l.pop_back();
        int sz = l.size();
        int rand_slot = rand_int(sz);
        l.remove(rand_slot);
    }

    unsigned int rand_seed = 1337; //hax
    //cop3530::ListFuzzer<int> fuzz(rand_seed, [](){return static_cast<int>(rand()) % 10;}, [](const int& a, const int &b) -> bool{return a==b;}); //;
    typedef std::string T;
    cop3530::ListFuzzer<T> fuzzer(rand_seed, rand_string, same_string); //;
    //cop3530::ListFuzzer<char> fuzzer(rand_seed, rand_char, same_char); //;
    for (int j = 0;; ++j) {
        if ( ! (j % 10000))
            std::cerr << "running test " << j << std::endl;
        fuzzer.test_lists();
    }
}

/*
int main() {
    fuzz();
    return 0;
}
*/
