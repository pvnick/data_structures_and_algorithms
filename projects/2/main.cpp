#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include "common/common.h"
#include "part1/source/open_addressing_map.h"
#include "part2/source/buckets_map.h"
#include "part3/source/part3.h"
#include <map>


int main() {
    size_t capacity = 10;
	cop3530::HashMapBuckets bucket_map(10);
	cop3530::HashMapOpenAddressing open_addr_map(10000);
	cop3530::HashMapOpenAddressingGeneric<int, char> generic_open_addr(10000);
	#define mymap generic_open_addr
	srand(1337);
	std::map<int, char> stdm;
    for (int i = 0; i != 100; ++i) {
        mymap.insert(i, 'a');
        stdm[i] = 'a';
    }
	for (int i = 0; i != 1000; ++i) {
        int key;
        char value;
        char value2;
        switch(rand() % 2) {
        case 0:
            //insert

            key = rand() % 20;
            value = 'a' + (rand() % 26);
            mymap.insert(key, value);
            stdm[key] = value;
            mymap.search(key, value2);
            if (stdm[key] != value2)
                std::cout << "wot" << std::endl;
            /*
            std::cout << "Inserting <" << key << "," << value << ">... ";
            if (map.insert(key, value))
                std::cout << "success" << std::endl;
            else
                std::cout << "fail" << std::endl;
            break;
            */
            break;
        case 1:
            //remove
            //std::cout << "start: " << std::endl;
            //map.print(std::cout) << std::endl;
            key = rand() % 20;
            //std::cout << "Removing key: " << key << "... ";
            //bool found = map.sequential_search(key);
            value2 = stdm[key];
            if (mymap.remove(key, value)){
                if (value2 != value) {
                    std::cout << "no match" << std::endl;
                }
                std::cout << "value " << (stdm[key] == value ? " does " : " does not ") << " match" << std::endl;
                stdm[key] = 0;
            }
            /*if (map.remove(key, value)) {
                if ( ! found)
                    std::cout << "shouldnt have found: " << value << std::endl;
            } else {
                if (found)
                    std::cout << "should have found" << std::endl;
            }*/
            //std::cout << "end: " << std::endl;
            //map.print(std::cout) << std::endl;
            break;
        }
    }
}


