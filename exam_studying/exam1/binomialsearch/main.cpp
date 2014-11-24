#include <iostream>
#include <vector>
#include <iterator>
#include <stdlib.h>
#include <algorithm>

using namespace std;


//project name type: this should be binery. requires ordered list. returns match pointer
//this is much easier than using arrays + offsets
int* bsearch(int* begin, int* end, int target) {
    int* begin_orig;
    int* mid;
    do {
        mid = begin + (end - begin) / 2;
        if (*mid == target) {
            return mid;
        } else if (*mid < target) {
            begin = mid + 1;
        } else {
            //i[mid] > target
            end = mid;
        }
    } while (end != begin);
    return nullptr;
}

int main()
{
    int arr[1000];
    for (int i = 0; i != 1000; arr[i] = rand(), ++i)
        if (i != 0)
            std::cout << arr[i - 1] << std::endl;
    std::sort(std::begin(arr), std::end(arr));

    for (int i = 0; i != 1000; ++i)
        if (i != 0)
            std::cout << arr[i - 1] << std::endl;

    std::cout << bsearch(std::begin(arr), std::end(arr), 1449228398) - std::begin(arr) << std::endl;
    return 0;
}
